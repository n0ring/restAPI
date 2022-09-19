CREATE TABLE items (
    id VARCHAR(255) PRIMARY KEY NOT NULL UNIQUE,
    parentId VARCHAR(255),
    url VARCHAR(255),
    size INT,
    type VARCHAR(255) NOT NULL,
    date VARCHAR(255) NOT NULL
);


CREATE TABLE "childrens" (
  "parrent_id" VARCHAR(255) NOT NULL,
  "child_id" VARCHAR(255) NOT NULL
);



-- test values
INSERT INTO "items" ("id", "parentid", "url", "size", "type", "date")
VALUES 
('1', '2', 'some/path', 123, 'FILE', '1992-05-28T21:12:01Z'),
('2', '0', 'some/path', 123, 'FOLDER', '1992-05-28T21:12:01Z'),
('3', '4', 'some/path', 123, 'FILE', '1992-05-28T21:12:01Z'),
('4', '0', 'some/path', 123, 'FOLDER', '1992-05-28T21:12:01Z');

INSERT INTO "childrens" ("parrent_id", "child_id")
VALUES ('2', '1'),
('4', '3');