#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  auto len = min( data.size(), available_capacity() );	// ȷ����д������ݳ���
  if ( len == 0 ) { // �����д������ݳ���Ϊ0��˵���Ѿ�д���ˣ��׳��쳣
    throw runtime_error( "�޿��ÿռ�!" );
  } else if ( len < data.size() ) { // �����д������ݳ���С�� data �ĳ��ȣ�˵��ֻ��д�벿������
    data.resize( len );             // �� data �ĳ��Ƚض�Ϊ��д��ĳ���
  }
  // �� data д�뵽 buffer ��
  buffer_view.emplace( data );
  buffer_data.emplace( move( data ) );
  // ������д������ݳ���
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
  return capacity_ - bytes_pushed_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
    return buffer_view.front();
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
    throw runtime_error( "�޿�������!" );
  }
  // �� buffer �е����ݵ���
  while ( len > 0 ) {
    auto& front = buffer_view.front();
    if ( len >= front.size() ) {
      len -= front.size();
      buffer_data.pop();
      buffer_view.pop();
    } else {
      front.remove_prefix( len );
      len = 0;
    }
  }
  // �����ѵ��������ݳ���
  bytes_popped_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  return writer().bytes_pushed() - bytes_popped();
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
