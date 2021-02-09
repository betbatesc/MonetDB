START TRANSACTION;
CREATE TABLE "t1" ("c0" DECIMAL(18,3),"c1" BINARY LARGE OBJECT NOT NULL,"c2" DECIMAL(18,3),CONSTRAINT "t1_c1_unique" UNIQUE ("c1"));
PREPARE SELECT DISTINCT (SELECT DISTINCT r'|m<v' FROM t1 WHERE ((t1.c0)<(?)) GROUP BY t1.c2, ?), ?, t1.c2 FROM t1 WHERE CAST(? AS BOOLEAN) LIMIT 2103332269785059850;
	-- Could not determine type for argument number 2
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0"("c0" INTERVAL MONTH,"c1" INTERVAL SECOND);
CREATE TABLE "t1"("c0" INTERVAL MONTH,"c1" INTERVAL SECOND);
CREATE TABLE "t2"("c0" INTERVAL MONTH,"c1" INTERVAL SECOND);

SELECT 1 FROM t1 JOIN t2 ON NOT (NOT (SELECT FALSE FROM t2));
	-- empty
SELECT ALL CAST(NOT ((DATE '1970-01-22') NOT IN (DATE '1970-01-04')) AS INT) as count FROM t0, t1 FULL OUTER JOIN t2 ON NOT (NOT ((SELECT DISTINCT FALSE FROM t2, t0, t1)));
	-- empty
ROLLBACK;

CREATE TABLE "t0" ("c0" DOUBLE PRECISION NOT NULL);
INSERT INTO "t0" VALUES (0.13492451886840173);
INSERT INTO "t0" VALUES (79004262);
INSERT INTO "t0" VALUES (1476461507);
CREATE TABLE "t1" ("c0" DOUBLE PRECISION);
INSERT INTO "t1" VALUES (973588428);
INSERT INTO "t1" VALUES (NULL);
INSERT INTO "t1" VALUES (0.39517295223772886);
INSERT INTO "t1" VALUES (NULL);

SELECT t0.c0 BETWEEN SYMMETRIC (SELECT t0.c0 FROM t0) AND t0.c0 FROM t0;
	--error, more than one row returned by a subquery used as an expression
SELECT CAST(NOT ((t0.c0) NOT BETWEEN SYMMETRIC ((SELECT DISTINCT t0.c0 FROM t0, t1)) AND (t0.c0)) AS INT) FROM t0;
	--error, more than one row returned by a subquery used as an expression
DROP TABLE t0;
DROP TABLE t1;

START TRANSACTION;
CREATE TABLE "t0" ("c0" DATE NOT NULL);
CREATE TABLE "t1" ("c1" INT);
CREATE TABLE "t2" ("c0" DATE NOT NULL);

SELECT 0 <= ANY(SELECT CASE INTERVAL '2' SECOND WHEN INTERVAL '6' SECOND THEN t1.c1 ELSE (SELECT t1.c1 FROM t1) END FROM t1) FROM t1;
	-- empty
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" BOOLEAN NOT NULL);
CREATE TABLE "t1" ("c0" DECIMAL(18,3));
CREATE TABLE "t2" ("c0" DECIMAL(18,3),"c2" DATE);
PREPARE (SELECT DISTINCT t0.c0, INTERVAL '1734780053' SECOND FROM t0, t1) UNION ALL (SELECT ?, ? FROM t2);
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" INTERVAL MONTH NOT NULL,CONSTRAINT "t0_c0_pkey" PRIMARY KEY ("c0"),CONSTRAINT "t0_c0_unique" UNIQUE ("c0"));
INSERT INTO "t0" VALUES (INTERVAL '2101098338' MONTH);
CREATE TABLE "t1" ("c0" INTERVAL MONTH,"c1" BOOLEAN);
CREATE TABLE "t2" ("c0" INTERVAL MONTH);

SELECT CAST(t1.c1 AS INT) FROM t1, t0 RIGHT OUTER JOIN t2 ON ((SELECT DISTINCT t1.c0 FROM t1, t0 WHERE FALSE)) NOT IN (t0.c0);
	-- empty
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" DATE,"c2" INTEGER);
CREATE TABLE "t1" ("c1" TIMESTAMP,"c2" INTEGER);
CREATE TABLE "t2" ("c0" DATE,"c1" TIMESTAMP,"c2" INTEGER);
PREPARE (SELECT ?, t1.c2 FROM t1, t0 WHERE (SELECT DISTINCT (t1.c2) BETWEEN ASYMMETRIC (?) AND (t1.c2) FROM t1 CROSS JOIN 
((SELECT DISTINCT 6.9089063E7, TRUE FROM t2 WHERE TRUE) EXCEPT (SELECT ALL 0.4, FALSE FROM t2, t1 INNER JOIN t0 ON FALSE)) AS sub0 WHERE FALSE)) INTERSECT DISTINCT (SELECT DISTINCT 0.2, ? FROM t0, t2 WHERE ?);
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" VARCHAR(156) NOT NULL);
CREATE TABLE "t1" ("c0" VARCHAR(156) NOT NULL);
select 1 from t0, t1 inner join t0 on true;
	--error table name "t0" specified more than once
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" CHAR(315));
INSERT INTO "t0" VALUES ('(y/l'), (''), ('*J');
CREATE TABLE "t1" ("c0" VARCHAR(512), "c1" DOUBLE PRECISION);
INSERT INTO "t1" VALUES ('0.9295919173154146', NULL);
CREATE TABLE "t2" ("c0" CHAR(315));
INSERT INTO "t2" VALUES ('1753268987.000'), ('(y/l'), ('(y/l'), ('');

SELECT '1' > COALESCE(((SELECT t1.c0 FROM t1) INTERSECT (SELECT '2')), t0.c0) FROM t0;
	-- True
	-- True
	-- True
SELECT CAST(((COALESCE(t0.c0, t0.c0, t0.c0))>(COALESCE(((SELECT DISTINCT t1.c0 FROM t1, t0, t2 WHERE FALSE) INTERSECT DISTINCT (SELECT ALL t0.c0 FROM t0 WHERE TRUE)), 
t0.c0, CASE INTERVAL '720497648' MONTH WHEN INTERVAL '1899785652' MONTH THEN t0.c0 ELSE t0.c0 END))) AS INT) FROM t0;
	-- 0
	-- 0
	-- 0
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" VARCHAR(156) NOT NULL);
INSERT INTO "t0" VALUES ('');
CREATE TABLE "t1" ("c0" VARCHAR(156) NOT NULL);
INSERT INTO "t1" VALUES ('i?#\t+U,s'), ('288044674'), ('b'), ('W?ykP7L+X'), ('34076821'), ('ah'), ('﹂9j0M4');

SELECT CASE '1' WHEN COALESCE((SELECT t0.c0 FROM t0), COALESCE(t1.c0, '2')) THEN 1 END FROM t1;
	-- 7 NULL rows
ROLLBACK;

CREATE TABLE "t0" ("c1" VARCHAR(388) NOT NULL,CONSTRAINT "t0_c1_pkey" PRIMARY KEY ("c1"),CONSTRAINT "t0_c1_unique" UNIQUE ("c1"));
INSERT INTO "t0" VALUES (''), ('Ral%}?U*A'), ('Dz '), ('P');
CREATE TABLE "t2" ("c1" VARCHAR(388) NOT NULL);
INSERT INTO "t2" VALUES ('4'),('4'),('3eSU8,'),(''),('5E~쟱'),('~'),('1386006226'),('0.19005213960704492'),('''{Mdd뒆VB'''),('\015␱%L%]'),('+'),(''),('㕚o+k');

select t0.c1 from t0 where (5) in (case when t0.c1 = 'a' then 1 end, (select 3));
	-- empty
select t0.c1 from t0 where (5) in ((select 3), case when t0.c1 = 'a' then 1 end);
	-- empty
select t0.c1 from t0 where (5) in (case when t0.c1 = 'a' then 1 end, (select 3 from t0));
	-- empty
select t0.c1 from t0 where (-5) in (case when t0.c1 = 'a' then 1 else -2 end, (select -3 from t0, t2 where false));
	-- empty
DROP TABLE t0;
DROP TABLE t2;

CREATE TABLE t1 (c0 DOUBLE PRECISION NOT NULL);
INSERT INTO t1(c0) VALUES((0.6548429615298178*0.3050008736497528)/(0.5061323979270875)); --error, scale too large
INSERT INTO t1(c0) VALUES(((((COALESCE(0.6548429615298178, 0.20317629894456002))*(COALESCE(0.3050008736497528, 0.2277902039419617))))/(0.5061323979270875))); --error, scale too large
SELECT t1.c0 FROM t1;
	-- empty
DROP TABLE t1;

START TRANSACTION;
CREATE TABLE "t0" ("c0" DECIMAL(18,3) NOT NULL,"c1" DOUBLE PRECISION NOT NULL);
CREATE TABLE "t1" ("c1" DOUBLE PRECISION);
CREATE TABLE "t2" ("c0" DECIMAL(18,3),"c1" DOUBLE PRECISION);

SELECT (SELECT 1 FROM t1, t2, t0 JOIN (SELECT 1) AS sub0 ON (t1.c1) BETWEEN (t2.c1) AND (t1.c1)) FROM t2, t0 CROSS JOIN t1;
	-- empty
ROLLBACK;

CREATE TABLE "t0" ("c0" BIGINT NOT NULL,CONSTRAINT "t0_c0_pkey" PRIMARY KEY ("c0"),CONSTRAINT "t0_c0_unique" UNIQUE ("c0"));
INSERT INTO "t0" VALUES (0),(-1557127883),(-488477810);
CREATE TABLE "t1" ("c0" BIGINT NOT NULL);
INSERT INTO "t1" VALUES (1457011207),(98933083),(1259938486);
CREATE TABLE "t2" ("c0" BIGINT NOT NULL,CONSTRAINT "t2_c0_pkey" PRIMARY KEY ("c0"));
INSERT INTO "t2" VALUES (596983192), (-601428889), (1688368391);

SELECT 1 FROM t2, t0 CROSS JOIN t1 WHERE t0.c0 % (SELECT 1 WHERE FALSE) <= t1.c0;
	-- empty
SELECT t1.c0 FROM t2, t0 CROSS JOIN t1 WHERE ((((t0.c0)%((SELECT DISTINCT t0.c0 FROM t1, t0, t2 WHERE FALSE))))<=(t1.c0));
	-- empty
SELECT CAST(SUM(count) AS BIGINT) FROM (SELECT ALL CAST(((((t0.c0)%((SELECT DISTINCT t0.c0 FROM t1, t0, t2 WHERE FALSE))))<=(t1.c0)) AS INT) as count FROM t2, t0 CROSS JOIN t1) as res;
	-- NULL
SELECT 1 FROM t2, t0 WHERE (SELECT 1 UNION SELECT 2) > 0;
	-- error, more than one row returned by a subquery used as an expression
SELECT 1 FROM t2, t0, t1 WHERE (SELECT 1 UNION SELECT 2) > 0;
	-- error, more than one row returned by a subquery used as an expression
SELECT 1 FROM t2, t0 CROSS JOIN t1 WHERE (SELECT 1 UNION SELECT 2) > 0;
	-- error, more than one row returned by a subquery used as an expression
DROP TABLE t0;
DROP TABLE t1;
DROP TABLE t2;

SELECT 1 WHERE (SELECT 1 WHERE FALSE) IS NULL;
	-- 1

PREPARE SELECT 1 WHERE greatest(true, ?);
	-- ? should be set to boolean

PREPARE SELECT (SELECT ? FROM (select 1) as v1(c0));
	-- cannot determine parameter type

PREPARE SELECT ?, CASE 'weHtU' WHEN (values (?)) THEN 'G' END;
	-- cannot determine parameter type

PREPARE SELECT DISTINCT ?, CAST(CASE least(?, r'weHtU') WHEN ? THEN ? WHEN ? THEN ? WHEN (VALUES (?)) THEN r'G' ELSE ? END AS DATE) WHERE (?) IS NOT NULL LIMIT 519007555986016405;
	-- cannot have a parameter for IS NOT NULL operator

START TRANSACTION;
CREATE TABLE "t0"("c0" INTEGER,"c1" DECIMAL(18,3));
INSERT INTO "t0" VALUES (-533312265, 0.254);

SELECT 1 FROM t0 WHERE EXISTS ((SELECT 0.59, TRUE) INTERSECT (SELECT t0.c0, TRUE));
	-- empty
UPDATE t0 SET c1 = 0.9, c0 = 53 WHERE EXISTS ((SELECT 0.5932837, (+ (46)) IS NULL FROM t0) INTERSECT (SELECT t0.c0, TRUE));
	-- no rows updated
ROLLBACK;

values (1,(select 2));
	-- 1 2
values (1,(select 2 intersect select 3));
	-- 1 NULL
values (1,(select 2 except select 2));
	-- 1 NULL

START TRANSACTION;
CREATE TABLE "t0" ("c0" INTEGER,"c1" DECIMAL(18,3));
INSERT INTO "t0" VALUES (72238796, 0.553);
CREATE TABLE "t1" ("c0" INTEGER);
INSERT INTO "t1" VALUES (-1302854973), (-1302854973), (107900469), (-292023894);
CREATE TABLE "t2" ("c0" INTEGER);
INSERT INTO "t2" VALUES (1), (2), (3);

SELECT t0.c1 FROM t0 WHERE (VALUES (t0.c1 = (SELECT t0.c1 FROM t0)));
	-- 0.553
SELECT CAST(sum((((VALUES (NULLIF(0, 1.09949409E8))))*(t0.c0))) AS BIGINT) FROM t2 JOIN t0 ON 
(VALUES ((t0.c1) NOT BETWEEN ASYMMETRIC (CAST(140698873 AS DECIMAL)) AND (((SELECT DISTINCT t0.c1 FROM t0, t1, t2) EXCEPT DISTINCT (SELECT t0.c1 FROM t0, t2 WHERE TRUE)))));
	-- 0
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t1" ("c0" DOUBLE PRECISION NOT NULL);

select 1 from (values (0.86), ((select 3 from t1))) as sub0(c0);
	-- 1
	-- 1
select sub0.c0 from (values (0.86), ((select 3 from t1))) as sub0(c0); -- this one is right
	-- 0.86
	-- NULL
select sub0.c0 from (values (0.86), ((select 3 from t1))) as sub0(c0) WHERE NOT TRUE;
	-- empty
select sub0.c0 from (values (0.86), ((select 3))) as sub0(c0);
	-- 0.86
	-- 3.00
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" DOUBLE PRECISION,"c1" INT);
CREATE TABLE "t1"("c1" INT);
INSERT INTO "t1" VALUES (1);

SELECT 1 FROM t0 WHERE (EXISTS (SELECT 1 WHERE FALSE)) IN ((SELECT FALSE)); -- rel_push_semijoin_down_or_up is causing the issue
	-- empty
SELECT t0.c0 FROM t0 WHERE (EXISTS (SELECT TRUE, 0.3, INTERVAL '5' MONTH, TIME '16:59:58' WHERE FALSE)) IN ((SELECT DISTINCT FALSE FROM t1), (('0.9')NOT ILIKE('')));
	-- empty
SELECT CAST(SUM(count) AS BIGINT) FROM (SELECT CAST((EXISTS (SELECT ALL TRUE, 0.3, INTERVAL '5' MONTH, TIME '16:59:58' WHERE FALSE)) IN ((SELECT DISTINCT FALSE FROM t1), (('0.9')NOT ILIKE(''))) AS INT) as count FROM t0) as res;
	-- NULL
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("tc0" TIMESTAMP);
INSERT INTO "t0" VALUES (TIMESTAMP '1970-01-07 07:00:31.000000');
create view v0(vc0) as (select 0.22400981);

SELECT t0.tc0 FROM v0 RIGHT OUTER JOIN t0 ON FALSE WHERE t0.tc0 = ANY(SELECT TIMESTAMP '1969-12-15 15:50:02'); -- rel_push_semijoin_down_or_up is causing the issue again
	-- empty
SELECT ALL t0.tc0 FROM v0 RIGHT OUTER JOIN t0 ON FALSE WHERE t0.tc0 = ANY(VALUES (ifthenelse(((TRUE)<=(TRUE)), TIMESTAMP '1969-12-15 15:50:02', TIMESTAMP '1970-01-12 06:29:27')));
	-- empty
SELECT CAST(SUM(count) AS BIGINT) FROM (SELECT ALL CAST(t0.tc0 = ANY(VALUES (ifthenelse(((TRUE)<=(TRUE)), TIMESTAMP '1969-12-15 15:50:02', TIMESTAMP '1970-01-12 06:29:27'))) AS INT) as count FROM v0 RIGHT OUTER JOIN t0 ON FALSE) as res;
	-- 0
ROLLBACK;

START TRANSACTION;
CREATE TABLE "t0" ("c0" TIMESTAMP NOT NULL,CONSTRAINT "t0_c0_pkey" PRIMARY KEY ("c0"),CONSTRAINT "t0_c0_unique" UNIQUE ("c0"));
COPY 5 RECORDS INTO "t0" FROM stdin USING DELIMITERS E'\t',E'\n','"';
"1970-01-06 01:52:11.000000"
"1970-01-14 10:55:10.000000"
"1970-01-04 18:13:57.000000"
"1970-01-03 16:27:13.000000"
"1970-01-07 23:49:20.000000"

CREATE TABLE "t1" ("c0" TIMESTAMP,CONSTRAINT "t1_c0_unique" UNIQUE ("c0"));
CREATE TABLE "t2" ("c0" TIMESTAMP NOT NULL,CONSTRAINT "t2_c0_pkey" PRIMARY KEY ("c0"));
COPY 5 RECORDS INTO "t2" FROM stdin USING DELIMITERS E'\t',E'\n','"';
"1970-01-18 20:11:31.000000"
"1970-01-17 19:38:53.000000"
"1970-01-04 22:47:27.000000"
"1970-01-19 03:36:31.000000"
"1970-01-06 00:11:21.000000"

create view v0(c0) as (values (((-839450346)||(0.2907855)))) with check option;
create view v1(c0) as (with cte0(c0) as (values (case when ((true)or(true)) then ((0.75)&(0.16181643)) else 0.5068248 end)) select all 0.48 from t1, v0, t2,t0,cte0);
create view v2(c0) as (select distinct interval '883554454' month from t1, t2) with check option;

VALUES ('b' LIKE (SELECT 'a' FROM t1)), (EXISTS (SELECT 1 WHERE TRUE));
	-- NULL
	-- True
SELECT (VALUES ((('524519669')NOT ILIKE((SELECT 'X囊*T\f' FROM t1, v2 WHERE FALSE)))), (NOT EXISTS (SELECT DISTINCT sql_min(0.52, 0.90) WHERE TRUE))) FROM v1, t2 RIGHT OUTER JOIN v0 ON ((v0.c0)<>('Vx'));
	-- empty
ROLLBACK;

CREATE TABLE t0(c0 BLOB DEFAULT (blob 'E443Aefab0') UNIQUE PRIMARY KEY NULL);
CREATE TABLE t2(LIKE t0);
INSERT INTO t2(c0) VALUES(blob ''), (NULL), (blob 'A0');
DELETE FROM t2 WHERE TRUE;
INSERT INTO t2(c0) VALUES(blob 'dAe8C7');
CALL sys.shrink('sys', 't2');
DROP TABLE t0;
DROP TABLE t2;

START TRANSACTION;
CREATE TABLE "t0" ("c0" INTERVAL MONTH NOT NULL,"c1" TINYINT NOT NULL,CONSTRAINT "t0_c0_c1_pkey" PRIMARY KEY ("c0", "c1"),CONSTRAINT "t0_c1_c0_unique" UNIQUE ("c1", "c0"));
INSERT INTO "t0" VALUES (INTERVAL '2076515258' MONTH, 125), (INTERVAL '2043534562' MONTH, 55), (INTERVAL '202288310' MONTH, 100), (INTERVAL '1514712125' MONTH, 101), (INTERVAL '708314220' MONTH, 116);
CREATE TABLE "t1" ("c0" BIGINT NOT NULL);
INSERT INTO "t1" VALUES (29358), (29358), (29358), (39231208), (-697168639), (3232261), (-379432950);
CREATE TABLE "t2" ("c0" BIGINT NOT NULL,CONSTRAINT "t2_c0_pkey" PRIMARY KEY ("c0"),CONSTRAINT "t2_c0_unique" UNIQUE ("c0"));
INSERT INTO "t2" VALUES (446453788), (1973525566), (1604369323), (-907882617), (1067873323), (1615259391), (1286378024);

SELECT 1 FROM t0 RIGHT OUTER JOIN t1 ON 3 IN ((SELECT 2), t1.c0);
	-- 7 1s
SELECT CAST(SUM(count) AS BIGINT) FROM (SELECT CAST(FALSE AS INT) as count FROM t2, t0 RIGHT OUTER JOIN t1 ON (CAST(1.2 AS DECIMAL)) IN 
	((SELECT DISTINCT 1832275178 FROM t0, t1), ((t1.c0)+(0.83835187465939664530623076643678359687328338623046875)))) as res;
	-- 0
ROLLBACK;

START TRANSACTION;
create view v0(vc0) as (values (interval '100' second));
create view v3(vc0) as (select 1638520390);
create view v4(vc0) as (values (-1548784118));

SELECT 1 FROM (SELECT 2) v4(vc0) WHERE (0.6) IN (CASE WHEN v4.vc0 THEN (SELECT 0.2 WHERE TRUE) END);
	-- empty
SELECT 1 FROM (SELECT 1) as v3(c0) RIGHT JOIN (SELECT 2) as v4(c0) ON (0.6) NOT IN (0.5, (SELECT 0.2));
	-- 1
SELECT * FROM (SELECT 1) as v3(c0) LEFT JOIN (SELECT 2) as v4(c0) ON (0.6) NOT IN ((SELECT v4.c0), 0.5);
	-- 1 2
SELECT FALSE FROM v3, v0 FULL OUTER JOIN v4 ON sql_max(FALSE, (0.6902854) NOT IN (0.5854332103580835, CASE WHEN v4.vc0 THEN (SELECT 0.023816515 WHERE TRUE) END));
	-- False
ROLLBACK;

SELECT CAST(((24829)+(((0.9767751031140547)*(0.7479400824095245)))) AS DOUBLE) IS NULL;
	-- error, overflow in conversion

PREPARE SELECT (1 + CAST(l0t0.c0 AS BIGINT)) * scale_up(?, 2) FROM (select 1) AS l0t0(c0);

PREPARE SELECT DISTINCT ((((CAST(l0t0.c0 AS INT))-(CAST(? AS BIGINT))))*(scale_up(?, ((-438139776)*(-813129345))))) FROM (select 1) AS l0t0(c0);

select interval '1' month * cast(1 as bigint);

START TRANSACTION;
create view v10(vc0) as (select l0v0.vc0 from (values (1, 2)) as l0v0(vc0, vc1));
prepare select case when true then false when ? then not exists (select ?, ?, 6) end from v10 as l0v10;
ROLLBACK;

prepare values (0.34, (values ((select ? from (select 1) as t1(x))))), (3, 2);

prepare with cte0(c0) as (select 1), cte1(c0,c1,c2) as (select distinct 1, 2, false)
select distinct least('y', (values (''), (''))), 1, (select ? from (select 1) as l1v0(x))
from cte0 as l0cte0, cte1 as l0cte1 order by l0cte0.c0 desc nulls last, l0cte1.c2 desc nulls first; -- ? can't be defined, so error

prepare with cte0(c0) as (select 2) select 1 <> all(select 2 from (values (1),(2)) as t1) from cte0 as l0cte0 group by ?; --error, cannot have a parameter for group by column

prepare with cte0(c0) as (select 2) select 1 <> all(select 2 from (values (1),(2)) as t1) from cte0 as l0cte0 order by ?; --error, cannot have a parameter for order by column
