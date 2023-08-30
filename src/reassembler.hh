/*****************************************************************//**
 * \file   reassembler.hh
 * \brief  实现一个 Reassembler 类, 用于将乱序的字符串重新组装成有序的
 *         字符串，并推入字节流.
 * 
 * \author JMC
 * \date   August 2023
 *********************************************************************/
#pragma once

#include "byte_stream.hh"

#include <string>
#include <list>
#include <tuple>

class Reassembler
{
	bool had_last_ {};	// 是否已经插入了最后一个字符串
	uint64_t next_index_ {};	// 下一个要写入的字节的索引
	uint64_t buffer_size_ {};	// buffer_中的字节数
	std::list<std::tuple<uint64_t, uint64_t, std::string>> buffer_ {};

	/**
	 * \breif 将data推入output流.
	 */
	void push_to_output(std::string data, Writer& output);

	/**
	 * \brief 将data推入buffer暂存区.
	 * \param first_index data的第一个字节的索引
	 * \param last_index  data的最后一个字节的索引
	 * \param data        待推入的字符串, 下标为[first_index, last_index]闭区间
	 */
	void buffer_push( uint64_t first_index, uint64_t last_index, std::string data );

	/**
	 * 尝试将buffer中的串推入output流.
	 */
	void buffer_pop(Writer& output);

public:
  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;
};
