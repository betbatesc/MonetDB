START TRANSACTION;

CREATE TABLE rapi13(a INTEGER, b DOUBLE);
INSERT INTO rapi13 VALUES (1,1.2), (1,2.2), (2,2.5), (2,0.9);


CREATE FUNCTION rapi13(a INTEGER, b DOUBLE) RETURNS TABLE (c INTEGER, d DOUBLE) LANGUAGE R {
data.frame(c=a,d=b)};

SELECT * FROM rapi13((SELECT A, B FROM rapi13 AS X));

CREATE VIEW rapi13v AS SELECT * FROM rapi13((SELECT A, B FROM rapi13 AS X));
SELECT * FROM rapi13v;

DROP VIEW rapi13v;
DROP FUNCTION rapi13;
DROP TABLE rapi13;

ROLLBACK;