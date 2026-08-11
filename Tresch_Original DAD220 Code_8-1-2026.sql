CREATE DATABASE QuantigrationRMA;
USE QuantigrationRMA;

/* ---------- 1. CREATE CUSTOMERS (classicmodels structure) ---------- */
CREATE TABLE Customers (
    CollaboratorID INT PRIMARY KEY,
    CustomerName VARCHAR(50),
    ContactLastName VARCHAR(50),
    ContactFirstName VARCHAR(50),
    Phone VARCHAR(50),
    AddressLine1 VARCHAR(50),
    AddressLine2 VARCHAR(50),
    City VARCHAR(50),
    State VARCHAR(50),
    PostalCode VARCHAR(15),
    Country VARCHAR(50),
    SalesRepEmployeeNumber INT,
    CreditLimit DECIMAL(10,2)
);

/* ---------- 2. INSERT SAMPLE ROWS (your original Module 3 data) ---------- */
INSERT INTO Customers VALUES
(101,'Savannah','Burnett','546 Amerige Rd.','New Milford','Connecticut',6776,'202-918-2132',NULL,NULL,NULL,NULL,NULL),
(102,'Darion','Hodges','9040 W. Cross Lane','Athens','Georgia',30605,'505-646-7448',NULL,NULL,NULL,NULL,NULL),
(103,'Paulina','Krueger','8186 Canterbury Dr','Canyon Country','California',91387,'205-654-0803',NULL,NULL,NULL,NULL,NULL),
(104,'Rosa','Nicholson','790 Piper Dr.','West Islip','New York',11795,'314-790-3768',NULL,NULL,NULL,NULL,NULL),
(105,'Aydin','Nixon','8174 Mayfair Ave.','Littleton','Colorado',80123,'505-646-7067',NULL,NULL,NULL,NULL,NULL),
(106,'Bryson','Bridges','8401 East Cherry Hill Ave.','Glendale','Arizona',85302,'319-371-4766',NULL,NULL,NULL,NULL,NULL),
(107,'Marques','Gibbs','722 Cardinal Drive','Hope Mills','North Carolina',28348,'505-282-1578',NULL,NULL,NULL,NULL,NULL),
(108,'Luz','Massey','194 Trenton St.','Muscatine','Iowa',52761,'207-898-2289',NULL,NULL,NULL,NULL,NULL),
(109,'Deandre','Pratt','49 Old Vernon St.','Edison','New Jersey',8817,'505-644-6257',NULL,NULL,NULL,NULL,NULL),
(110,'Claudia','Gentry','33 Shady Street','Willingboro','New Jersey',8046,'505-692-0842',NULL,NULL,NULL,NULL,NULL);

/* ---------- 3. RENAME CUSTOMERS → COLLABORATORS ---------- */
ALTER TABLE Customers RENAME Collaborators;

/* ---------- 4. CREATE ORDERS (AFTER rename!) ---------- */
CREATE TABLE Orders (
    OrderID INT PRIMARY KEY,
    OrderDate DATE,
    RequiredDate DATE,
    ShippedDate DATE,
    Status VARCHAR(20),
    Comments VARCHAR(255),
    CollaboratorID INT,
    FOREIGN KEY (CollaboratorID) REFERENCES Collaborators(CollaboratorID)
);

/* ---------- 5. CREATE RMA ---------- */
CREATE TABLE RMA (
    RMAID VARCHAR(20) PRIMARY KEY,
    OrderID INT,
    CollaboratorID INT,
    RequestDate DATE,
    ApprovedDate DATE,
    ReturnedDate DATE,
    ResolvedDate DATE,
    Reason VARCHAR(50),
    Status VARCHAR(20),
    RefundAmount DECIMAL(10,2),
    Comments VARCHAR(255),
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
    FOREIGN KEY (CollaboratorID) REFERENCES Collaborators(CollaboratorID)
);

/* ---------- 6. LOAD customers.csv ---------- */
LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\customers.csv'
INTO TABLE Collaborators
FIELDS TERMINATED BY ','
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(CollaboratorID, CustomerName, ContactLastName, ContactFirstName, Phone,
 AddressLine1, AddressLine2, City, State, @PostalCode, Country,
 @SalesRepEmployeeNumber, @CreditLimit)
SET PostalCode = NULLIF(@PostalCode, ''),
    SalesRepEmployeeNumber = NULLIF(@SalesRepEmployeeNumber, ''),
    CreditLimit = NULLIF(@CreditLimit, '');

/* ---------- 7. LOAD orders.csv ---------- */
LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\orders.csv'
INTO TABLE Orders
FIELDS TERMINATED BY ','
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(OrderID, @OrderDate, @RequiredDate, @ShippedDate, Status, Comments, CollaboratorID)
SET OrderDate = NULLIF(@OrderDate, ''),
    RequiredDate = NULLIF(@RequiredDate, ''),
    ShippedDate = NULLIF(@ShippedDate, '');

/* ---------- 8. LOAD rma.csv ---------- */
LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\rma.csv'
INTO TABLE RMA
FIELDS TERMINATED BY ','
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(RMAID, OrderID, CollaboratorID,
 @RequestDate, @ApprovedDate, @ReturnedDate, @ResolvedDate,
 Reason, Status, @RefundAmount, Comments)
SET RequestDate = NULLIF(@RequestDate, ''),
    ApprovedDate = NULLIF(@ApprovedDate, ''),
    ReturnedDate = NULLIF(@ReturnedDate, ''),
    ResolvedDate = NULLIF(@ResolvedDate, ''),
    RefundAmount = NULLIF(@RefundAmount, '');
    
/* ---------- Module 4---------- */
SELECT * FROM Collaborators;
SELECT * FROM Orders;
SELECT * FROM RMA;

SELECT 
    O.OrderID,
    O.OrderDate,
    O.Status,
    C.CustomerName,
    C.ContactLastName,
    C.ContactFirstName
FROM Orders O
JOIN Collaborators C
    ON O.CollaboratorID = C.CollaboratorID;

SELECT
    R.RMAID,
    R.OrderID,
    R.Status AS RMAStatus,
    R.Reason,
    R.RefundAmount,
    O.OrderDate,
    C.CustomerName,
    C.ContactLastName,
    C.ContactFirstName
FROM RMA R
JOIN Orders O
    ON R.OrderID = O.OrderID
JOIN Collaborators C
    ON R.CollaboratorID = C.CollaboratorID;

SELECT
    C.CollaboratorID,
    C.CustomerName,
    O.OrderID,
    O.Status
FROM Collaborators C
LEFT JOIN Orders O
    ON C.CollaboratorID = O.CollaboratorID
ORDER BY C.CollaboratorID;

SELECT
    C.CustomerName,
    COUNT(R.RMAID) AS TotalRMAs
FROM Collaborators C
LEFT JOIN RMA R
    ON C.CollaboratorID = R.CollaboratorID
GROUP BY C.CustomerName
ORDER BY TotalRMAs DESC;

SELECT
    C.CustomerName,
    COUNT(O.OrderID) AS TotalOrders
FROM Collaborators C
LEFT JOIN Orders O
    ON C.CollaboratorID = O.CollaboratorID
GROUP BY C.CustomerName
ORDER BY TotalOrders DESC;

SELECT
    C.CustomerName,
    SUM(R.RefundAmount) AS TotalRefunds
FROM Collaborators C
JOIN RMA R
    ON C.CollaboratorID = R.CollaboratorID
GROUP BY C.CustomerName
ORDER BY TotalRefunds DESC;

CREATE OR REPLACE VIEW CustomerOrderSummary AS
SELECT
    C.CollaboratorID,
    C.CustomerName,
    COUNT(O.OrderID) AS TotalOrders,
    COUNT(R.RMAID) AS TotalRMAs,
    SUM(R.RefundAmount) AS TotalRefundAmount
FROM Collaborators C
LEFT JOIN Orders O
    ON C.CollaboratorID = O.CollaboratorID
LEFT JOIN RMA R
    ON C.CollaboratorID = R.CollaboratorID
GROUP BY C.CollaboratorID, C.CustomerName;

SELECT * FROM CustomerOrderSummary;

CREATE OR REPLACE VIEW RMADetails AS
SELECT
    R.RMAID,
    R.OrderID,
    R.Status AS RMAStatus,
    R.Reason,
    R.RefundAmount,
    O.OrderDate,
    C.CustomerName,
    C.ContactLastName,
    C.ContactFirstName
FROM RMA R
JOIN Orders O
    ON R.OrderID = O.OrderID
JOIN Collaborators C
    ON R.CollaboratorID = C.CollaboratorID;

SELECT * FROM RMADetails;

/* ---------- Module 5---------- */
SELECT 
    Collaborators.State,
    COUNT(DISTINCT Collaborators.CollaboratorID) AS Customer_Base
FROM Collaborators
JOIN Orders ON Orders.CollaboratorID = Collaborators.CollaboratorID
GROUP BY Collaborators.State
ORDER BY Customer_Base DESC
LIMIT 5;

ALTER TABLE Orders
ADD COLUMN SKU VARCHAR(50),
ADD COLUMN Description VARCHAR(255);

SELECT 
    COUNT(*) AS Product_Sales_Number,
    SKU AS Product_SKU,
    Description AS Product_Description
FROM Orders
GROUP BY SKU, Description
ORDER BY Product_Sales_Number DESC
LIMIT 3;

SELECT 
    COUNT(*) AS Product_Sales_Number,
    Orders.SKU AS Product_SKU,
    Orders.Description AS Product_Description
FROM Orders
JOIN Collaborators ON Orders.CollaboratorID = Collaborators.CollaboratorID
WHERE UPPER(Collaborators.State) IN ('VIRGINIA','NORTH CAROLINA','SOUTH CAROLINA','GEORGIA')
GROUP BY Orders.SKU, Orders.Description
ORDER BY Product_Sales_Number DESC
LIMIT 3;

SELECT 
    COUNT(*) AS Returned_Amount,
    Orders.SKU AS Product_SKU,
    Orders.Description AS Product_Description
FROM Orders
JOIN RMA ON Orders.OrderID = RMA.OrderID
WHERE UPPER(RMA.Status) = 'COMPLETE'
GROUP BY Orders.SKU
ORDER BY Returned_Amount DESC
LIMIT 3;

SELECT 
    COUNT(*) AS Returned_Amount,
    Orders.SKU AS Product_SKU,
    Orders.Description AS Product_Description
FROM Orders
JOIN RMA ON Orders.OrderID = RMA.OrderID
JOIN Customers ON Orders.CustomerID = Customers.CustomerID
WHERE UPPER(Customers.State) IN ('WASHINGTON','OREGON','IDAHO','MONTANA')
  AND UPPER(RMA.Status) = 'COMPLETE'
GROUP BY Orders.SKU
ORDER BY Returned_Amount DESC
LIMIT 3;

SELECT 
    Orders.SKU,
    Orders.Description AS Product_Description,
    COUNT(*) * 100.0 / (SELECT COUNT(*) FROM Orders) AS Sales_Percentage
FROM Orders
GROUP BY Orders.SKU
ORDER BY Sales_Percentage DESC
LIMIT 3;

SELECT 
    Orders.SKU,
    Orders.Description AS Product_Description,
    COUNT(*) * 100.0 / (SELECT COUNT(*) FROM RMA) AS Returns_Percentage
FROM Orders
JOIN RMA ON Orders.OrderID = RMA.OrderID
GROUP BY Orders.SKU, Orders.Description
ORDER BY Returns_Percentage DESC
LIMIT 3;

SELECT 
    O.SKU,
    O.Description AS Product_Description,
    COUNT(*) * 100.0 / (SELECT COUNT(*) FROM Orders) AS Return_Percentage,
    R.Reason
FROM Orders O
JOIN RMA R ON O.OrderID = R.OrderID
WHERE O.SKU IN ('BAS-48-1 C','ENT-48-40F','ENT-24-10F')
GROUP BY O.SKU, O.Description, R.Reason
ORDER BY Return_Percentage DESC;

/* ---------- Project 1---------- */
CREATE TABLE Collaborators(
CustomerID INT,
FirstName VARCHAR(25),
LastName VARCHAR(25),
Street VARCHAR(50),
City VARCHAR(50),
State VARCHAR(25),
ZipCode INT,
Telephone VARCHAR(15),
PRIMARY KEY (CustomerID)
);

CREATE TABLE Orders(
OrderID int,
CustomerID INT,
SKU VARCHAR(20),
Description VARCHAR(50),
PRIMARY KEY (OrderID)
);

CREATE TABLE RMA (
RMAID int,
OrderID int,
Step VARCHAR(50),
Status VARCHAR(15),
Reason VARCHAR(15),
PRIMARY KEY (RMAID)
); 

ALTER TABLE Customers
CHANGE CustomerID CollaboratorID INT;

ALTER TABLE Orders
CHANGE CustomerID CollaboratorID INT;

INSERT INTO Customers
Values (100004, 'Luke', 'Skywalker', '15 Maiden Lane', 'New York', 'New York', 10222, '212-555-1234'),
(100005, 'Winston', 'Smith', '123 Sycamore Street', 'Greensboro', 'North Carolina', 27401, '919-555-6623'),
(100006, 'MaryAnne', 'Jenkins', '1 Coconut Way', 'Jupiter', 'Florida', 33458, '321-555-8907'),
(100007, 'Janet', 'Williams', '55 Redondo Beach Blvd', 'Torrence', 'California', 90501, '310-555-5678');

INSERT INTO Orders
Values (1204305, 100004, 'ADV-24-10C', 'Advanced Switch 10GigE Copper 24 port'),
(1204306, 100005, 'ADV-48-10F',	'Advanced Switch 10 GigE Copper/Fiber 44 port copper 4 port fiber'),
(1204307, 100006, 'ENT-24-10F',	'Enterprise Switch 10GigE SFP+ 24 Port'),
(1204308, 100007, 'ENT-48-10F',	'Enterprise Switch 10GigE SFP+ 48 port');

SELECT Count(*)
FROM Customers
WHERE City LIKE '%Woonsocket%';

/* ---------- Project 2---------- */
CREATE DATABASE QuantigrationUpdates;
USE QuantigrationUpdates;

CREATE TABLE Customers (
    CustomerNumber INT PRIMARY KEY,
    CustomerName VARCHAR(100),
    ContactLastName VARCHAR(50),
    ContactFirstName VARCHAR(50),
    Phone VARCHAR(25),
    AddressLine1 VARCHAR(100),
    AddressLine2 VARCHAR(100),
    City VARCHAR(50),
    State VARCHAR(50),
    PostalCode VARCHAR(15),
    Country VARCHAR(50),
    SalesRepEmployeeNumber INT,
    CreditLimit DECIMAL(10,2)
);

CREATE TABLE Orders (
    OrderNumber INT PRIMARY KEY,
    OrderDate DATE,
    RequiredDate DATE,
    ShippedDate DATE,
    Status VARCHAR(50),
    Comments VARCHAR(255),
    CustomerNumber INT,
    FOREIGN KEY (CustomerNumber) REFERENCES Customers(CustomerNumber)
);

CREATE TABLE RMA (
    RMANumber VARCHAR(20) PRIMARY KEY,
    OrderNumber INT,
    CustomerNumber INT,
    RequestDate DATE,
    ApprovedDate DATE,
    ReturnedDate DATE,
    ResolvedDate DATE,
    Reason VARCHAR(50),
    Status VARCHAR(25),
    RefundAmount DECIMAL(10,2),
    Comments VARCHAR(255),
    FOREIGN KEY (OrderNumber) REFERENCES Orders(OrderNumber),
    FOREIGN KEY (CustomerNumber) REFERENCES Customers(CustomerNumber)
);

INSERT INTO Customers VALUES
(101,'Savannah','Burnett','546 Amerige Rd.','New Milford','Connecticut',6776,'202-918-2132',NULL,NULL,NULL,NULL,NULL),
(102,'Darion','Hodges','9040 W. Cross Lane','Athens','Georgia',30605,'505-646-7448',NULL,NULL,NULL,NULL,NULL),
(103,'Paulina','Krueger','8186 Canterbury Dr','Canyon Country','California',91387,'205-654-0803',NULL,NULL,NULL,NULL,NULL),
(104,'Rosa','Nicholson','790 Piper Dr.','West Islip','New York',11795,'314-790-3768',NULL,NULL,NULL,NULL,NULL),
(105,'Aydin','Nixon','8174 Mayfair Ave.','Littleton','Colorado',80123,'505-646-7067',NULL,NULL,NULL,NULL,NULL),
(106,'Bryson','Bridges','8401 East Cherry Hill Ave.','Glendale','Arizona',85302,'319-371-4766',NULL,NULL,NULL,NULL,NULL),
(107,'Marques','Gibbs','722 Cardinal Drive','Hope Mills','North Carolina',28348,'505-282-1578',NULL,NULL,NULL,NULL,NULL),
(108,'Luz','Massey','194 Trenton St.','Muscatine','Iowa',52761,'207-898-2289',NULL,NULL,NULL,NULL,NULL),
(109,'Deandre','Pratt','49 Old Vernon St.','Edison','New Jersey',8817,'505-644-6257',NULL,NULL,NULL,NULL,NULL),
(110,'Claudia','Gentry','33 Shady Street','Willingboro','New Jersey',8046,'505-692-0842',NULL,NULL,NULL,NULL,NULL);

ALTER TABLE Customers RENAME Collaborators;

LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\customers.csv'
INTO TABLE Collaborators
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(CustomerNumber, CustomerName, ContactLastName, ContactFirstName, Phone,
 AddressLine1, AddressLine2, City, State, @PostalCode, Country,
 @SalesRepEmployeeNumber, @CreditLimit)
SET PostalCode = NULLIF(@PostalCode, ''),
    SalesRepEmployeeNumber = NULLIF(@SalesRepEmployeeNumber, ''),
    CreditLimit = NULLIF(@CreditLimit, '');

LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\orders.csv'
INTO TABLE Orders
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(@OrderNumber, @OrderDate, @RequiredDate, @ShippedDate, @Status, @Comments, @CustomerNumber)
SET OrderNumber = NULLIF(@OrderNumber, ''),
    OrderDate = NULLIF(@OrderDate, ''),
    RequiredDate = NULLIF(@RequiredDate, ''),
    ShippedDate = NULLIF(@ShippedDate, ''),
    Status = NULLIF(@Status, ''),
    Comments = NULLIF(@Comments, ''),
    CustomerNumber = NULLIF(@CustomerNumber, '');
    
LOAD DATA INFILE 'C:\\ProgramData\\MySQL\\MySQL Server 8.0\\Uploads\\rma.csv'
INTO TABLE RMA
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\r\n'
IGNORE 1 LINES
(@RMANumber, @OrderNumber, @CustomerNumber,
 @RequestDate, @ApprovedDate, @ReturnedDate, @ResolvedDate,
 @Reason, @Status, @RefundAmount, @Comments)
SET RMANumber = NULLIF(@RMANumber, ''),
    OrderNumber = NULLIF(@OrderNumber, ''),
    CustomerNumber = NULLIF(@CustomerNumber, ''),
    RequestDate = NULLIF(@RequestDate, ''),
    ApprovedDate = NULLIF(@ApprovedDate, ''),
    ReturnedDate = NULLIF(@ReturnedDate, ''),
    ResolvedDate = NULLIF(@ResolvedDate, ''),
    Reason = NULLIF(@Reason, ''),
    Status = NULLIF(@Status, ''),
    RefundAmount = NULLIF(@RefundAmount, ''),
    Comments = NULLIF(@Comments, '');
    
SELECT COUNT(*) FROM Collaborators;
SELECT COUNT(*) FROM Orders;
SELECT COUNT(*) FROM RMA;

SELECT 
    Collaborators.State,
    COUNT(*) AS Return_Number
FROM RMA
JOIN Orders ON RMA.OrderNumber = Orders.OrderNumber
JOIN Collaborators ON Orders.CustomerNumber = Collaborators.CustomerNumber
GROUP BY Collaborators.State
ORDER BY Return_Number DESC;

SELECT 
    RMA.Status,
    COUNT(*) AS TotalReturns,
    (COUNT(*) / (SELECT COUNT(*) FROM RMA) * 100) AS Percentage_of_Returns
FROM RMA
GROUP BY RMA.Status
ORDER BY TotalReturns DESC;

SELECT 
    RMA.Reason,
    COUNT(*) AS TotalReturns,
    (COUNT(*) / (SELECT COUNT(*) FROM RMA) * 100) AS Percentage_of_Returns
FROM RMA
WHERE UPPER(RMA.Status) = 'COMPLETED'
GROUP BY RMA.Reason
ORDER BY Percentage_of_Returns ASC
LIMIT 10;

SELECT 
    (SELECT COUNT(*) FROM Orders) AS Number_of_Orders,
    COUNT(*) AS Number_of_Returns,
    COUNT(*) / (SELECT COUNT(*) FROM Orders) * 100 AS Percentage_of_Orders_Returned
FROM Orders
JOIN RMA ON Orders.OrderNumber = RMA.OrderNumber
WHERE UPPER(RMA.Status) = 'COMPLETED';

SELECT Reason, COUNT(*) AS Total_Returns
FROM RMA
GROUP BY Reason

ORDER BY Total_Returns DESC;
