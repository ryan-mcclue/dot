-- sqlite> .headers on
-- sqlite> .mode csv
-- sqlite> .output test.csv
-- sqlite> select * from tbl1;
-- sqlite> .output stdout
-- IMPORTANT(Ryan): USE EXCEL FOR GRAPH CREATION OVER GNUPLOT!

create table if not exists build_machines (
  hash text primary key,
  os text not null,
  cpu text not null,
  compiler text not null,
  linker text not null
);

create table if not exists build_metrics (
  created_at timestamp not null default CURRENT_TIMESTAMP,
  name text not null,
  branch text not null,
  hash text not null,
  parent_hash text not null,
  build_type text not null,
  text_size integer not null,
  data_size integer not null,
  bss_size integer not null,
  build_time real not null,
  loc integer not null,
  sym0_name text not null,
  sym0_val integer not null,
  sym1_name text not null,
  sym1_val integer not null,
  sym2_name text not null,
  sym2_val integer not null,
  sym3_name text not null,
  sym3_val integer not null,
  sym4_name text not null,
  sym4_val integer not null,
  sym5_name text not null,
  sym5_val integer not null,
  sym6_name text not null,
  sym6_val integer not null,
  sym7_name text not null,
  sym7_val integer not null,
  sym8_name text not null,
  sym8_val integer not null,
  sym9_name text not null,
  sym9_val integer not null,
  build_machine text references build_machines(text),
  constraint hash_different check (parent_hash != hash),
  primary key (name, hash, build_type)
);
