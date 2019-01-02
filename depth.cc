#include "depth.h"

void Depth::serialize_to_protobuf(redepth::Depth& pb) {
    pb.set_updated_at(boost::posix_time::to_iso_extended_string(updated_at_));
    for (std::pair<Decimal, PriceLevelPtr> elm : bids_) {
        redepth::PriceLevel *pl = pb.add_bids();
        elm.second->serialize_to_protobuf(*pl);
    }

    for (std::pair<Decimal, PriceLevelPtr> elm : asks_) {
        redepth::PriceLevel *pl = pb.add_asks();
        elm.second->serialize_to_protobuf(*pl);
    }
}
