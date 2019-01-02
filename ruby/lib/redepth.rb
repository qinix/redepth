require 'redis'
require 'redepth/version'
require 'redepth/depth_pb'
require 'redepth/price_level'
require 'redepth/depth'
require 'redepth/mixin'

module Redepth
  class << self
    def redis=(conn)
      @redis = conn
    end

    def redis
      @redis || $redis || Redis.current || raise('Redepth.redis not set to a Redis.new connection')
    end
  end
end

Redis.include Redepth::Mixin
