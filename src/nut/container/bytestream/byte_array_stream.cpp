﻿
#include <assert.h>

#include "byte_array_stream.h"

namespace nut
{

ByteArrayStream::ByteArrayStream()
{
    _data = rc_new<byte_rcarray_type>(16);
}

ByteArrayStream::ByteArrayStream(byte_rcarray_type *arr)
    : _data(arr)
{
    assert(nullptr != arr);
}

size_t ByteArrayStream::size() const
{
    return _data->size();
}

void ByteArrayStream::resize(size_t new_size)
{
    _data->resize(new_size);
    if (_index > new_size)
        _index = new_size;
}

ByteArrayStream::byte_rcarray_type* ByteArrayStream::byte_array() const
{
    return _data;
}

bool ByteArrayStream::is_little_endian() const
{
    return _little_endian;
}

void ByteArrayStream::set_little_endian(bool le)
{
    _little_endian = le;
}

size_t ByteArrayStream::length() const
{
    return _data->size();
}

size_t ByteArrayStream::tell() const
{
    return _index;
}

size_t ByteArrayStream::available() const
{
    return _data->size() - _index;
}

void ByteArrayStream::seek(size_t index)
{
    assert(index <= _data->size());
    _index = index;
}

void ByteArrayStream::skip(size_t cb)
{
    assert(_index + cb <= _data->size());
    _index += cb;
}

uint8_t ByteArrayStream::read_uint8()
{
    assert(available() >= sizeof(uint8_t));
    const byte_rcarray_type *data = _data;
    return data->at(_index++);
}

size_t ByteArrayStream::read(void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    const byte_rcarray_type *data = _data;
    size_t ret = (cb < available() ? cb : available());
    ::memcpy(buf, data->data() + _index, ret);
    _index += ret;
    return ret;
}

void ByteArrayStream::write_uint8(uint8_t v)
{
    if (_index < _data->size())
        _data->operator[](_index) = v;
    else
        _data->push_back(v);
    ++_index;
}

size_t ByteArrayStream::write(const void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    assert(_index <= _data->size());
    const size_t copy = _data->size() - _index;
    ::memcpy(_data->data() + _index, buf, copy);
    _data->append(((const uint8_t*) buf) + copy, ((const uint8_t*) buf) + cb - copy);
    _index += cb;
    return cb;
}

size_t ByteArrayStream::write(const byte_array_type& ba)
{
    return write(ba.data(), ba.size());
}

}
