drop table if exists Bid;
drop table if exists User;
drop table if exists Seller;
drop table if exists Bidder;
drop table if exists ItemNC;
drop table if exists ItemWC;

CREATE TABLE User (
  UserID TEXT PRIMARY KEY,
  Country TEXT,
  Location TEXT,
  Rating REAL
);
CREATE TABLE ItemNC (
  ItemID INTEGER PRIMARY KEY,
  Name TEXT,
  Currently REAL,
  BuyPrice REAL,
  FirstBid REAL,
  Nbids INTEGER,
  Description TEXT,
  Started TIME,
  Ends TIME
);
CREATE TABLE ItemWC (
  ItemID INTEGER,
  Category TEXT,
  FOREIGN KEY (ItemID) REFERENCES ItemNC(ItemID),
  PRIMARY KEY(ItemID, Category)
);
CREATE TABLE Bid (
  Seller TEXT,
  Bidder TEXT,
  Merch INTEGER,
  Tiem TIME,
  Amount REAL,
  FOREIGN KEY (Seller) REFERENCES User(UserID),
  FOREIGN KEY (Bidder) REFERENCES User(UserID),
  FOREIGN KEY (Merch) REFERENCES ItemNC(ItemID),
  PRIMARY KEY(Seller,Bidder,Merch,Tiem)
);
CREATE TABLE Seller (
  UserID TEXT,
  ItemID INTEGER,
  FOREIGN KEY (UserID) REFERENCES User(UserID),
  FOREIGN KEY (ItemID) REFERENCES ItemNC(ItemID),
  PRIMARY KEY(UserID, ItemID)
);
CREATE TABLE Bidder (
  UserID TEXT,
  ItemID INTEGER,
  FOREIGN KEY (UserID) REFERENCES User(UserID),
  FOREIGN KEY (ItemID) REFERENCES ItemNC(ItemID),
  PRIMARY KEY(UserID, ItemID)
);
