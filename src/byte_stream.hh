#pragma once

#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <cstdint>

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
  std::queue<std::string> buffer_data {};
  std::string_view buffer_view {};

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
  void push( std::string data ) noexcept; // �ڿ�����������ķ�Χ��������д������

  void close() noexcept; // �ر�������������������д������
  void set_error() noexcept; // ���г��ִ�����λ�����־

  bool is_closed() const noexcept;      // �ж����Ƿ��ѹر�
  uint64_t available_capacity() const noexcept; // ��������ʣ���������
  uint64_t bytes_pushed() const noexcept;       // ����������д����ֽ���
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const noexcept; // ����������һ�����ݿ��ֻ����ͼ
  void pop( uint64_t len ) noexcept;      // �����е���ָ�����ȵ����ݿ�

  bool is_finished() const noexcept; // �ж����Ƿ��ѹر����������ݿ鶼�ѵ���
  bool has_error() const noexcept;   // �ж����Ƿ���ִ���

  uint64_t bytes_buffered() const noexcept; // ���㵱ǰ����ʣ����ֽ���
  uint64_t bytes_popped() const noexcept;   // ���������ѵ������ֽ���
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );
