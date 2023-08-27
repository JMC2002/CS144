#pragma once

#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>

class Reader;
class Writer;

class ByteStream
{
protected:
  enum State { CLOSED, ERROR };
  uint64_t capacity_;
  uint64_t bytes_pushed_ {}; // ��д����ֽ���
  uint64_t bytes_popped_ {}; // �ѵ������ֽ���

  unsigned char flag {};	// 0: normal, 1: closed, 2: error
  std::deque<std::string> buffer_data {};
  std::queue<std::string_view> buffer_view {};

public:
  explicit ByteStream( uint64_t capacity );

  // �ṩByteStream�� reader �� writer �ӿڵĸ�������
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;
};

class Writer : public ByteStream
{
public:
  void push( std::string data ); // �ڿ�����������ķ�Χ��������д������

  void close();     // �ر�������������������д������
  void set_error(); // ���г��ִ�����λ�����־

  bool is_closed() const;              // �ж����Ƿ��ѹر�
  uint64_t available_capacity() const; // ��������ʣ���������
  uint64_t bytes_pushed() const;       // ����������д����ֽ���
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const; // ����������һ�����ݿ��ֻ����ͼ
  void pop( uint64_t len );      // �����е���ָ�����ȵ����ݿ�

  bool is_finished() const; // �ж����Ƿ��ѹر����������ݿ鶼�ѵ���
  bool has_error() const;   // �ж����Ƿ���ִ���

  uint64_t bytes_buffered() const; // ���㵱ǰ����ʣ����ֽ���
  uint64_t bytes_popped() const;   // ���������ѵ������ֽ���
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );
