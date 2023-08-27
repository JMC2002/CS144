#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  auto len = min( data.size(), available_capacity() );	// 确定可写入的数据长度
  if ( len == 0 ) { // 如果可写入的数据长度为0，说明已经写满了，返回
    return;
  } else if ( len < data.size() ) { // 如果可写入的数据长度小于 data 的长度，说明只能写入部分数据
    data.resize( len );             // 将 data 的长度截断为可写入的长度
  }
  // 将 data 写入到 buffer 中
  buffer_data.emplace_back( move( data ) );
  buffer_view.emplace( buffer_data.back() );
  // 更新已写入的数据长度
  bytes_pushed_ += len;
}

void Writer::close()
{
  flag |= ( 1 << CLOSED );
}

void Writer::set_error()
{
  flag |= ( 1 << ERROR );
}

bool Writer::is_closed() const
{
  return flag & ( 1 << CLOSED );
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  return buffer_view.empty() ? ""sv : buffer_view.front();
}

bool Reader::is_finished() const
{
  return writer().is_closed() && ( bytes_buffered() == 0 );
}

bool Reader::has_error() const
{
  return flag & ( 1 << ERROR );
}

void Reader::pop( uint64_t len )
{
  if ( len > bytes_buffered() ) {
    throw runtime_error( "可用数据数量不够!" );
  }
  // 更新已弹出的数据长度
  bytes_popped_ += len;

  // 将 buffer 中的数据弹出
  while ( len > 0 ) {
    auto& front = buffer_view.front();
    if ( len >= front.size() ) {
      len -= front.size();
      buffer_data.pop_front();
      buffer_view.pop();
    } else {
      front.remove_prefix( len );
      len = 0;
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  return writer().bytes_pushed() - bytes_popped();
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
