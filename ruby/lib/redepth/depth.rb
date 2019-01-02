require 'time'

module Redepth
  class Depth
    attr_accessor :key, :asks, :bids, :updated_at

    def self.from_pb(pb)
      new.tap do |depth|
        depth.asks = pb.asks.map(&PriceLevel.method(:from_pb))
        depth.bids = pb.bids.map(&PriceLevel.method(:from_pb))
        depth.updated_at = Time.iso8601(pb.updated_at)
      end
    end

    def self.find(key)
      Redepth.redis.redepth_get(key)
    end

    def refetch!
      newdepth = Redepth.redis.redepth_get(key)
      self.asks = newdepth.asks
      self.bids = newdepth.bids
      self.updated_at = newdepth.updated_at
      self
    end

    def add(price, amount, side)
      Redepth.redis.redepth_add(key, price, amount, side)
      refetch!
    end

    def fill(price, amount, side, is_fully_filled)
      Redepth.redis.redepth_fill(key, price, amount, side, is_fully_filled)
      refetch!
    end

    def close(price, amount, side)
      Redepth.redis.redepth_close(key, price, amount, side)
      refetch!
    end

    def merge(depth)
      Redepth.redis.redepth_merge(key, depth)
      refetch!
    end

    def newchanges
      Redepth.redis.redepth_newchanges(key)
    end
  end
end
