#include "reassembler.hh"

using namespace std;
void Reassembler::buffer_push( uint64_t first_index, uint64_t last_index, std::string data )
{
  auto l = first_index, r = last_index;
  auto beg = buffer_.begin(), end = buffer_.end();
  auto lef = lower_bound( beg, end, l, []( auto& a, auto& b ) { return get<1>( a ) < b; } );
  auto rig = upper_bound( lef, end, r, []( auto& b, auto& a ) { return get<0>( a ) > b; } );
  if (lef != end) l = min( l, get<0>( *lef ) );
  if (rig != end) r = max( r, get<1>( *prev( rig ) ) );
  buffer_size_ += r - l;

  if (data.size() == r - l) {	// 说明buffer_中没有data重叠的部分
	buffer_.emplace( rig, l, r, move( data ) );
	return;
  }
  string s( r - l, 0 );
  // for (auto it = lef; it != rig; ++it) {
  for ( auto&& it : views::iota( lef, rig ) ) {
	auto& [a, b, c] = *it;
	buffer_size_ -= c.size();
    ranges::copy(c, s.begin() + a - l);
  }
  ranges::copy(data, s.begin() + first_index - l);
  buffer_.emplace( buffer_.erase( lef, rig ), l, r, move( s ) );
}

void Reassembler::buffer_pop( Writer& output ) {
  while ( !buffer_.empty() && get<0>( buffer_.front() ) == next_index_ ) {
    auto& [a, b, c] = buffer_.front();
    output.push( move( c ) );
    next_index_ = b;
    buffer_size_ -= c.size();
    buffer_.pop_front();
  }

  if ( had_last_ && buffer_.empty() ) {
    output.close();
  }
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  //had_last_ |= is_last_substring;
  if ( data.empty() ) {
    if ( is_last_substring ) {
      output.close();
    }
    return;
  }
  auto end_index = first_index + data.size();                  // data: [first_index, end_index)
  auto last_index = next_index_ + output.available_capacity(); // 可用范围: [next_index_, last_index)
  if ( last_index <= next_index_ || first_index >= last_index ) {
    return; // 不在可用范围内, 直接返回
  }

  // 调整data的范围
  if ( last_index > end_index ) {
    data.resize( end_index - first_index );
    is_last_substring = false;
  }
  if ( first_index < next_index_ ) {
    data = data.substr( next_index_ - first_index );
    first_index = next_index_;
  }

  // 将data插入buffer_
  buffer_push(first_index, end_index, data);
  had_last_ |= is_last_substring;

  // 将buffer_中的数据写入output
  buffer_pop(output);
}

uint64_t Reassembler::bytes_pending() const
{
  return buffer_size_;
}
