// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit
// Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiocore/WaveFile.h"

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-module/audiocore/WaveFile.h"

#define BITS_PER_SAMPLE 8

struct wave_format_chunk {
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  uint16_t extra_params; // Assumed to be zero below.
};

void common_hal_audioio_wavefile_set_stretch(audioio_wavefile_obj_t *self,
                                             const uint32_t stretch_factor) {
  // self->stretch_factor = stretch_factor;
}

void common_hal_audioio_wavefile_construct(audioio_wavefile_obj_t *self,
                                           pyb_file_obj_t *file,
                                           uint8_t *buffer,
                                           size_t buffer_size) {
  // self->stretch_factor = 100;
  // Load the wave
  self->file.handle = file;
  uint8_t chunk_header[16];
  FIL *fp = &self->file.handle->fp;
  f_rewind(fp);
  UINT bytes_read;
  if (f_read(fp, chunk_header, 16, &bytes_read) != FR_OK) {
    mp_raise_OSError(MP_EIO);
  }
  if (bytes_read != 16 || memcmp(chunk_header, "RIFF", 4) != 0 ||
      memcmp(chunk_header + 8, "WAVEfmt ", 8) != 0) {
    mp_arg_error_invalid(MP_QSTR_file);
  }
  uint32_t format_size;
  if (f_read(fp, &format_size, 4, &bytes_read) != FR_OK) {
    mp_raise_OSError(MP_EIO);
  }
  if (bytes_read != 4 || format_size > sizeof(struct wave_format_chunk)) {
    mp_raise_ValueError(MP_ERROR_TEXT("Invalid format chunk size"));
  }
  struct wave_format_chunk format;
  if (f_read(fp, &format, format_size, &bytes_read) != FR_OK) {
    mp_raise_OSError(MP_EIO);
  }
  if (bytes_read != format_size) {
  }

  if (format.audio_format != 1 || format.num_channels > 1 ||
      format.bits_per_sample != 8 ||
      (format_size == 18 && format.extra_params != 0)) {
    mp_raise_ValueError(MP_ERROR_TEXT("Unsupported format"));
  }
  // Get the sample_rate
  self->sample_rate = format.sample_rate;

  // TODO(tannewt): Skip any extra chunks that occur before the data section.

  uint8_t data_tag[4];
  if (f_read(fp, &data_tag, 4, &bytes_read) != FR_OK) {
    mp_raise_OSError(MP_EIO);
  }
  if (bytes_read != 4 || memcmp((uint8_t *)data_tag, "data", 4) != 0) {
    mp_raise_ValueError(MP_ERROR_TEXT("Data chunk must follow fmt chunk"));
  }

  uint32_t data_length;
  if (f_read(fp, &data_length, 4, &bytes_read) != FR_OK) {
    mp_raise_OSError(MP_EIO);
  }
  if (bytes_read != 4) {
    mp_arg_error_invalid(MP_QSTR_file);
  }
  self->file.length = data_length;
  self->file.data_start = fp->fptr;

  // Try to allocate two buffers, one will be loaded from file and the other
  // DMAed to DAC.
  if (buffer_size) {
    self->max_buffer_length = buffer_size / 2;
    self->buffer1.data = buffer;
    self->buffer1.length = self->max_buffer_length / 2;
    self->buffer2.data = buffer + self->max_buffer_length;
    self->buffer2.length = self->max_buffer_length / 2;
  } else {
    self->max_buffer_length = 256;
    self->buffer1.data = m_malloc(self->max_buffer_length);
    self->buffer1.length = self->max_buffer_length;
    if (self->buffer1.data == NULL) {
      common_hal_audioio_wavefile_deinit(self);
      m_malloc_fail(self->max_buffer_length);
    }

    self->buffer2.data = m_malloc(self->max_buffer_length);
    self->buffer2.length = self->max_buffer_length;
    if (self->buffer2.data == NULL) {
      common_hal_audioio_wavefile_deinit(self);
      m_malloc_fail(self->max_buffer_length);
    }
  }
}

void common_hal_audioio_wavefile_deinit(audioio_wavefile_obj_t *self) {
  self->buffer1.data = NULL;
  self->buffer2.data = NULL;
}

bool common_hal_audioio_wavefile_deinited(audioio_wavefile_obj_t *self) {
  return self->buffer1.data == NULL;
}

uint32_t
common_hal_audioio_wavefile_get_sample_rate(audioio_wavefile_obj_t *self) {
  return self->sample_rate;
}

void common_hal_audioio_wavefile_set_sample_rate(audioio_wavefile_obj_t *self,
                                                 uint32_t sample_rate) {
  self->sample_rate = sample_rate;
}

uint8_t
common_hal_audioio_wavefile_get_bits_per_sample(audioio_wavefile_obj_t *self) {
  return BITS_PER_SAMPLE;
}

uint8_t
common_hal_audioio_wavefile_get_channel_count(audioio_wavefile_obj_t *self) {
  return 1;
}

void audioio_wavefile_reset_buffer(audioio_wavefile_obj_t *self,
                                   bool single_channel_output,
                                   uint8_t _channel) {
  // We don't reset the buffer index in case we're looping and we have an odd
  // number of buffer loads
  self->file.bytes_remaining = self->file.length;
  f_lseek(&self->file.handle->fp, self->file.data_start);
}

static uint32_t add_padding(uint8_t *buffer, UINT length_read) {
  uint32_t pad_count = length_read % sizeof(uint32_t);
  for (uint32_t i = 0; i < pad_count; i++) {
    ((uint8_t *)(buffer))[length_read / sizeof(uint8_t) - i - 1] = 0x80;
  }

  return pad_count;
}

static Buffer *get_indexed_buffer(audioio_wavefile_obj_t *self,
                                  const uint8_t index) {
  if (index % 2 == 1) {
    return &self->buffer2;
  } else {
    return &self->buffer1;
  }
}

audioio_get_buffer_result_t
audioio_wavefile_get_buffer(audioio_wavefile_obj_t *self,
                            const bool single_channel_output, uint8_t _channel,
                            uint8_t **buffer, uint32_t *buffer_length) {

  if (self->file.bytes_remaining == 0) {
    *buffer = NULL;
    *buffer_length = 0;
    return GET_BUFFER_DONE;
  }

  struct Buffer *target_buffer = get_indexed_buffer(self, self->buffer_index);

  const uint32_t bytes_to_read =
      (self->max_buffer_length > self->file.bytes_remaining)
          ? self->file.bytes_remaining
          : self->max_buffer_length;

  UINT read_count;
  if (f_read(&self->file.handle->fp, target_buffer->data, bytes_to_read,
             &read_count) != FR_OK ||
      read_count != bytes_to_read) {
    return GET_BUFFER_ERROR;
  }

  self->file.bytes_remaining -= read_count;

  // Pad the last buffer to word align it.
  const bool is_last_buffer =
      self->file.bytes_remaining == 0 && read_count % sizeof(uint32_t) != 0;
  if (is_last_buffer) {
    read_count += add_padding(target_buffer->data, read_count);
  }

  target_buffer->length = read_count;

  struct Buffer *out_buffer = get_indexed_buffer(self, self->buffer_index + 1);

  *buffer = out_buffer->data;
  *buffer_length = out_buffer->length;
  self->buffer_index += 1;

  return self->file.bytes_remaining == 0 ? GET_BUFFER_DONE
                                         : GET_BUFFER_MORE_DATA;
}

void audioio_wavefile_get_buffer_structure(audioio_wavefile_obj_t *self,
                                           bool _single_channel_output,
                                           bool *single_buffer,
                                           bool *samples_signed,
                                           uint32_t *max_buffer_length,
                                           uint8_t *spacing) {
  *single_buffer = false;
  // In WAV files, 8-bit samples are always unsigned, and larger samples are
  // always signed.
  *samples_signed = false;
  *max_buffer_length = 512;
  *spacing = 1;
}
