CREATE TABLE IF NOT EXISTS test (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255)
);

-- Insert 100 data here
DO $$
BEGIN
    FOR i IN 1..100 LOOP
        INSERT INTO test (name) VALUES ('Data ' || i);
    END LOOP;
END $$;

-- Query all data
SELECT * FROM test;
