#include "tcp_receiver.hh"
#include <cstdint>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  if (!isn_) {
    if ( message.SYN )
      isn_ = message.seqno;
    else
      return;
  }
  
  auto const checkpoint = inbound_stream.bytes_pushed() + 1;
  auto const abs_seqno = message.seqno.unwrap( *isn_, checkpoint );
  auto const first_index = message.SYN ? 0 : abs_seqno - 1;
  reassembler.insert( first_index, message.payload.release(), message.FIN, inbound_stream );
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  return { isn_.transform( [&]( auto&& isn ) {
            return Wrap32::wrap( inbound_stream.bytes_pushed() + 1 + inbound_stream.is_closed(), isn );
          } ),
           (uint16_t)min<uint64_t>( inbound_stream.available_capacity(), UINT16_MAX ) };
}