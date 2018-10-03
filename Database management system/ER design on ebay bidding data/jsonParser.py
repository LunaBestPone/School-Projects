
"""
FILE: skeleton_parser.py
------------------
Author: Firas Abuzaid (fabuzaid@stanford.edu)
Author: Perth Charernwattanagul (puch@stanford.edu)
Modified: 04/21/2014

Skeleton parser for CS564 programming project 1. Has useful imports and
functions for parsing, including:

1) Directory handling -- the parser takes a list of eBay json files
and opens each file inside of a loop. You just need to fill in the rest.
2) Dollar value conversions -- the json files store dollar value amounts in
a string like $3,453.23 -- we provide a function to convert it to a string
like XXXXX.xx.
3) Date/time conversions -- the json files store dates/ times in the form
Mon-DD-YY HH:MM:SS -- we wrote a function (transformDttm) that converts to the
for YYYY-MM-DD HH:MM:SS, which will sort chronologically in SQL.

Your job is to implement the parseJson function, which is invoked on each file by
the main function. We create the initial Python dictionary object of items for
you; the rest is up to you!
Happy parsing!
"""

import sys
from json import loads
from re import sub

columnSeparator = "|"

# Dictionary of months used for date transformation
MONTHS = {'Jan':'01','Feb':'02','Mar':'03','Apr':'04','May':'05','Jun':'06',\
        'Jul':'07','Aug':'08','Sep':'09','Oct':'10','Nov':'11','Dec':'12'}

"""
Returns true if a file ends in .json
"""
def isJson(f):
    return len(f) > 5 and f[-5:] == '.json'

"""
Converts month to a number, e.g. 'Dec' to '12'
"""
def transformMonth(mon):
    if mon in MONTHS:
        return MONTHS[mon]
    else:
        return mon

"""
Transforms a timestamp from Mon-DD-YY HH:MM:SS to YYYY-MM-DD HH:MM:SS
"""
def transformDttm(dttm):
    dttm = dttm.strip().split(' ')
    dt = dttm[0].split('-')
    date = '20' + dt[2] + '-'
    date += transformMonth(dt[0]) + '-' + dt[1]
    return date + ' ' + dttm[1]

"""
Transform a dollar value amount from a string like $3,453.23 to XXXXX.xx
"""

def transformDollar(money):
    if money == None or len(money) == 0:
        return money
    return sub(r'[^\d.]', '', money)

"""
Parses a single json file. Currently, there's a loop that iterates over each
item in the data set. Your job is to extend this functionality to create all
of the necessary SQL tables for your database.
"""
def parseJson(json_file):
    """initiating load files needed"""
    g = open('ItemNC.dat','a')
    h = open('ItemWC.dat','a')
    j = open('User.dat','a')
    k = open('SellingHistory.dat','a')
    l = open('BiddingHistory.dat','a')
    z = open('Bid.dat','a')
    with open(json_file, 'r') as f:
        items = loads(f.read())['Items'] # creates a Python dictionary of Items for the supplied json file
        for item in items:
            """
            TODO: traverse the items dictionary to extract information from the
            given `json_file' and generate the necessary .dat files to generate
            the SQL tables based on your relation design
            """
            pass
            ItemID = item["ItemID"]

            """Category here is a list, need a for loop to deal with it"""
            Category = item["Category"]

            Ends = item["Ends"]
            FirstBid = transformDollar(item["First_Bid"])
            Name = item["Name"]
            Started = transformDttm(item["Started"])
            SellerCountry = item["Country"]
            NBids = item["Number_of_Bids"]
            Currently = transformDollar(item["Currently"])

            """BuyPrice might be missing"""
            if(list(item).count("Buy_Price") == 1):
                BuyPrice = transformDollar(item["Buy_Price"])
            else:
                BuyPrice = "missing"
            SellerLocation = item["Location"]
            SellerUserID =item["Seller"]["UserID"]
            SellerRating = item["Seller"]["Rating"]
            if(item["Description"] != None):
                Description = item["Description"]
            else:
                Description = "missing"

            """Bids(another list) should be here"""
            if(item["Bids"] != None):
                for bid in item["Bids"]:
                    Amount = transformDollar(bid["Bid"]["Amount"])
                    BidderRating = bid["Bid"]["Bidder"]["Rating"]
                    BidderUserID = bid["Bid"]["Bidder"]["UserID"]
                    if(list(bid["Bid"]["Bidder"]).count("Location") == 1):
                        BidderLocation = bid["Bid"]["Bidder"]["Location"]
                    else:
                        BidderLocation = "missing"
                    if(list(bid["Bid"]["Bidder"]).count("Country") == 1):
                        BidderCountry = bid["Bid"]["Bidder"]["Country"]
                    else:
                        BidderCountry = "missing"
                    Time = transformDttm(bid["Bid"]["Time"])

                    BidderLocationn = BidderLocation.replace('\"','\"\"')
                    j.write(BidderUserID + "|" + BidderCountry + "|\"" + BidderLocationn + "\"|"
                    + BidderRating + "\n")
                    l.write(BidderUserID + "|" + ItemID + "\n")
                    z.write(SellerUserID + "|" + BidderUserID + "|" + ItemID + "|"
                    + Time + "|" + Amount + "\n")

            """load file generation, some part of this code is moved to above lines"""
            Namen = Name.replace('\"','\"\"')
            Descriptionn = Description.replace('\"','\"\"')
            g.write(ItemID + "|\"" + Namen + "\"|" + Currently + "|" + BuyPrice + "|"
            + FirstBid + "|" + NBids + "|\"" + Descriptionn + "\"|" + Started + "|"
            + Ends + "\n")
            for SubCat in Category:
                h.write(ItemID + "|" + SubCat + "\n")
            SellerLocationn = SellerLocation.replace('\"','\"\"')
            j.write(SellerUserID + "|" + SellerCountry + "|\"" + SellerLocationn + "\"|"
            + SellerRating + "\n")
            k.write(SellerUserID + "|" + ItemID + "\n")
    g.close()
    h.close()
    j.close()
    k.close()
    l.close()
    z.close()

"""
Loops through each json files provided on the command line and passes each file
to the parser
"""
def main(argv):
    if len(argv) < 2:
        print >> sys.stderr, 'Usage: python skeleton_json_parser.py <path to json files>'
        sys.exit(1)
    # loops over all .json files in the argument
    for f in argv[1:]:
        if isJson(f):
            parseJson(f)
            print "Success parsing " + f

if __name__ == '__main__':
    main(sys.argv)
