#!/bin/bash
python skeleton_parser.py items-*.json
sort Bid.dat | uniq > Bid1.dat
sort User.dat | uniq > User1.dat
sort SellingHistory.dat | uniq > SellingHistory1.dat
sort BiddingHistory.dat | uniq > BiddingHistory1.dat
sort ItemNC.dat | uniq > ItemNC1.dat
sort ItemWC.dat | uniq > ItemWC1.dat
