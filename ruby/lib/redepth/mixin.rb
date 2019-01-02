module Redepth
  module Mixin
    def redepth_add(key, price, amount, side)
      redepth_check_side(side)
      synchronize do |client|
        client.call([:'redepth.add', key, price.to_s, amount.to_s, side.to_s.upcase])
      end
    end

    def redepth_fill(key, price, amount, side, is_fully_filled)
      redepth_check_side(side)
      synchronize do |client|
        client.call([:'redepth.fill', key, price.to_s, amount.to_s, side.to_s.upcase, is_fully_filled.to_s.upcase])
      end
    end

    def redepth_close(key, price, amount, side)
      redepth_check_side(side)
      synchronize do |client|
        client.call([:'redepth.close', key, price.to_s, amount.to_s, side.to_s.upcase])
      end
    end

    def redepth_get(key, limit = nil)
      rep = synchronize do |client|
        req = [:'redepth.get', key]
        req << limit.to_s if limit
        client.call(req)
      end
      Redepth::Depth.from_pb(Redepth::Proto::Depth.decode(rep)).tap do |d|
        d.key = key
      end
    end

    def redepth_merge(key, depth)
      raise ArgumentError unless depth.is_a?(Redepth::Proto::Depth)
      data = Redepth::Depth.encode(depth)
      synchronize do |client|
        client.call([:'redepth.merge', data])
      end
    end

    def redepth_newchanges(key)
      rep = synchronize do |client|
        client.call([:'redepth.newchanges', key])
      end
      Redepth::Depth.from_pb(Redepth::Proto::Depth.decode(rep)).tap do |d|
        d.key = key
      end
    end

    private
    def redepth_check_side(side)
      raise ArgumentError unless [:BID, :ASK, :bid, :ask, 'BID', 'ASK', 'bid', 'ask'].include?(side)
    end
  end
end
