#include "depth.h"

void Depth::add(Decimal price, Decimal amount, bool is_bid) {
    update_at(price, is_bid, [&](PriceLevelPtr pl) {
                                 pl->add_order(amount);
                             });
}

void Depth::fill(Decimal price, Decimal filled_amount, bool is_bid, bool is_fully_filled) {
    update_at(price, is_bid, [&](PriceLevelPtr pl) {
                                 if (is_fully_filled) {
                                     pl->close_order(filled_amount);
                                 } else {
                                     pl->decrease_amount(filled_amount);
                                 }
                             });
}

void Depth::close(Decimal price, Decimal amount, bool is_bid) {
    update_at(price, is_bid, [&](PriceLevelPtr pl) {
                                 pl->close_order(amount);
                             });
}

void Depth::merge(Depth newdepth) {
    for (auto item : newdepth.bids_) {
        auto bid = item.second;
        update_at(bid->price, true, [&](PriceLevelPtr pl) {
                pl->set(bid->amount, bid->order_count);
            });
    }

    for (auto item : newdepth.asks_) {
        auto ask = item.second;
        update_at(ask->price, false, [&](PriceLevelPtr pl) {
                pl->set(ask->amount, ask->order_count);
            });
    }
}

std::string Depth::newchanges() {
    Depth subdepth;
    for(PriceLevelPtr pl : changes_) {
        subdepth.at(pl->price, pl->is_bid)->set(pl->amount, pl->order_count);
    };
    changes_.clear();
    return subdepth.to_protobuf();
}

std::string Depth::to_protobuf(uint64_t limit) {
    redepth::Depth pb;
    serialize_to_protobuf(pb, limit);
    std::string str;
    pb.SerializeToString(&str);
    return str;
}

void Depth::serialize_to_protobuf(redepth::Depth& pb, uint64_t limit) {
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

void Depth::load_from_pb(std::string pbstr) {
    redepth::Depth pb;
    pb.ParseFromString(pbstr);

    for(redepth::PriceLevel ask : pb.asks()) {
        Decimal price(ask.price()), amount(ask.amount());
        update_at(price, false, [&](PriceLevelPtr pl) {
                                    pl->amount = amount;
                                    pl->order_count = ask.order_count();
                                });
    }

    for(redepth::PriceLevel bid : pb.bids()) {
        Decimal price(bid.price()), amount(bid.amount());
        update_at(price, true, [&](PriceLevelPtr pl) {
                                   pl->amount = amount;
                                   pl->order_count = bid.order_count();
                               });
    }

}

PriceLevelPtr Depth::at(Decimal price, bool is_bid) {
    if (is_bid) {
        auto search = bids_.find(price);
        if (search != bids_.end()) {
            return search->second;
        } else {
            bids_.emplace(price, std::make_shared<PriceLevel>(price, is_bid));
            return at(price, is_bid);
        }
    } else {
        auto search = asks_.find(price);
        if (search != asks_.end()) {
            return search->second;
        } else {
            asks_.emplace(price, std::make_shared<PriceLevel>(price, is_bid));
            return at(price, is_bid);
        }
    }
}

void Depth::remove_price_level_on_empty(PriceLevelPtr pl) {
    if (pl->order_count == 0 && pl->amount == 0) {
        if (pl->is_bid) {
            bids_.erase(pl->price);
        } else {
            asks_.erase(pl->price);
        }
    }
}

void Depth::update_at(Decimal price, bool is_bid, const std::function<void(PriceLevelPtr)>& block) {
    PriceLevelPtr pl = at(price, is_bid);
    block(pl);
    changes_.insert(pl);
    remove_price_level_on_empty(pl);
    updated_at_ = boost::posix_time::microsec_clock::universal_time();
}
