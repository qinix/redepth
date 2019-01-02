require 'bigdecimal'

module Redepth
  class PriceLevel
    attr_accessor :price, :amount, :order_count

    def self.from_pb(pb)
      new.tap do |pl|
        pl.price = BigDecimal(pb.price)
        pl.amount = BigDecimal(pb.amount)
        pl.order_count = pb.order_count
      end
    end
  end
end
