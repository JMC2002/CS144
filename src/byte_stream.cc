#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data ) noexcept
{
  auto len = min( data.size(), available_capacity() );	// ȷ����д������ݳ���
  if ( len == 0 ) { // �����д������ݳ���Ϊ0��˵���Ѿ�д���ˣ�����
    return;
  } else if ( len < data.size() ) { // �����д������ݳ���С�� data �ĳ��ȣ�˵��ֻ��д�벿������
    data.resize( len );             // �� data �ĳ��Ƚض�Ϊ��д��ĳ���
  }
  // �� data д�뵽 buffer ��
  buffer_data.emplace_back( move( data ) );
  buffer_view.emplace( buffer_data.back() );
  // ������д������ݳ���
  bytes_pushed_ += len;
}

void Writer::close() noexcept
{
  flag |= ( 1 << CLOSED );
}

void Writer::set_error() noexcept
{
  flag |= ( 1 << ERROR );
}

bool Writer::is_closed() const noexcept
{
  return flag & ( 1 << CLOSED );
}

uint64_t Writer::available_capacity() const noexcept
{
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const noexcept
{
  return bytes_pushed_;
}

string_view Reader::peek() const noexcept
{
  return buffer_view.empty() ? ""sv : buffer_view.front();
}

bool Reader::is_finished() const noexcept
{
  return writer().is_closed() && ( bytes_buffered() == 0 );
}

bool Reader::has_error() const noexcept
{
  return flag & ( 1 << ERROR );
}

void Reader::pop( uint64_t len ) noexcept
{
  if ( len > bytes_buffered() ) {
    return;
  }
  // �����ѵ��������ݳ���
  bytes_popped_ += len;

  // �� buffer �е����ݵ���
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

uint64_t Reader::bytes_buffered() const noexcept
{
  return writer().bytes_pushed() - bytes_popped();
}

uint64_t Reader::bytes_popped() const noexcept
{
  return bytes_popped_;
}
