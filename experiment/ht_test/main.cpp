/* 
 * File:   main.cpp
 * Author: phrk
 *
 * Created on November 25, 2013, 7:41 PM
 */

#include <cstdlib>
#include <boost/version.hpp>
#include "Common/Compat.h"
#include "Common/System.h"

#include <iostream>
#include <fstream>
#include "ThriftBroker/Client.h"
#include "ThriftBroker/gen-cpp/HqlService.h"
#include "ThriftBroker/ThriftHelper.h"
#include "ThriftBroker/SerializedCellsReader.h"

using namespace Hypertable;
using namespace Hypertable::ThriftGen;

void run(Thrift::Client *client);
void test_rename_alter(Thrift::Client *client, std::ostream &out);
void test_guid(Thrift::Client *client, std::ostream &out);
void test_unique(Thrift::Client *client, std::ostream &out);
void test_hql(Thrift::Client *client, std::ostream &out);
void test_scan(Thrift::Client *client, std::ostream &out);
void test_scan_keysonly(Thrift::Client *client, std::ostream &out);
void test_set(Thrift::Client *client);
void test_schema(Thrift::Client *client, std::ostream &out);
void test_put(Thrift::Client *client);
void test_async(Thrift::Client *client, std::ostream &out);
void test_error(Thrift::Client *client, std::ostream &out);

int main(int argc, char** argv)
{	
	Thrift::Client *client = new Thrift::Client("localhost", 38080);
	run(client);
	return 0;
}


void run(Thrift::Client *client) {
  try {
    std::ostream &out = std::cout;
/*    out << "running test_guid" << std::endl;
    test_guid(client, out);
    out << "running test_unique" << std::endl;
    test_unique(client, out);
    out << "running test_hql" << std::endl;
    test_hql(client, out);
    out << "running test_schema" << std::endl;
    test_schema(client, out);
*/    out << "running test_scan" << std::endl;
    test_scan(client, out);
/*    out << "running test_set" << std::endl;
    test_set(client);
    out << "running test_put" << std::endl;
    test_put(client);
    out << "running test_scan" << std::endl;
    test_scan(client, out);
    out << "running test_async" << std::endl;
    test_async(client, out);
    out << "running test_rename_alter" << std::endl;
    test_rename_alter(client, out);
    out << "running test_error" << std::endl;
    test_error(client, out);
    out << "running test_scan_keysonly" << std::endl;
    test_scan_keysonly(client, out);
*/  }
  catch (ClientException &e) {
    std::cout << e << std::endl;
    exit(1);
  }
}

void test_rename_alter(Thrift::Client *client, std::ostream &out) {
  out << "Rename and alter" << std::endl;
  if (!client->namespace_exists("test"))
  client->namespace_create("test");
  Namespace ns = client->namespace_open("test");
  HqlResult result;
  client->hql_query(result, ns, "drop table if exists foo");
  client->hql_query(result, ns, "drop table if exists foo_renamed");
  client->hql_query(result, ns, "create table foo('bar')");
  client->rename_table(ns, "foo", "foo_renamed");
  String str = (String)"<Schema generation=\"2\">" +
    "  <AccessGroup name=\"default\">" +
    "    <ColumnFamily id=\"1\">" +
    "      <Generation>1</Generation>" +
    "      <Name>bar</Name>" +
    "      <Counter>false</Counter>" +
    "      <deleted>true</deleted>" +
    "    </ColumnFamily>" +
    "    <ColumnFamily id=\"2\">" +
    "      <Generation>2</Generation>" +
    "      <Name>\"bar2\"</Name>" +
    "      <Counter>false</Counter>" +
    "      <deleted>false</deleted>" +
    "    </ColumnFamily>" +
    "   </AccessGroup>" +
    "</Schema>";
  client->alter_table(ns, "foo_renamed", str);
  client->get_schema_str_with_ids(str, ns, "foo_renamed");
  out << str << std::endl;
  client->drop_table(ns, "foo_renamed", false);
  client->namespace_close(ns);
}

void test_guid(Thrift::Client *client, std::ostream &out) {
  String s;
  client->generate_guid(s);
  out << "generate guid: " << s << std::endl;
}


void test_unique(Thrift::Client *client, std::ostream &out) {
  HqlResult result;
  if (!client->namespace_exists("test"))
    client->namespace_create("test");
  Namespace ns = client->namespace_open("test");
  client->hql_query(result, ns, "drop table if exists UniqueTest");
  client->hql_query(result, ns, "create table UniqueTest (cf1 TIME_ORDER DESC "
                    "MAX_VERSIONS 1, cf2)");
  out << result << std::endl;

  Hypertable::ThriftGen::Key key;
  key.column_family="cf1";
  String ret, value;
   
  key.row="row1";
  client->generate_guid(value);
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  key.row="row2";
  client->generate_guid(value);
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  key.row="row3";
  client->generate_guid(value);
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  key.row="row4";
  client->generate_guid(value);
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  key.row="row5";
  client->generate_guid(value);
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  value="";
  key.row="row6";
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  assert(!ret.empty());
  value="";
  key.row="row7";
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  assert(!ret.empty());
  value="";
  key.row="row8";
  client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  assert(!ret.empty());
  bool caught=false;
  try {
    client->generate_guid(value);
    client->create_cell_unique(ret, ns, "UniqueTest", key, value);
  }
  catch (ClientException &e) {
    caught=true;
  }
  assert(caught);

  client->namespace_close(ns);
}


void test_hql(Thrift::Client *client, std::ostream &out) {
  HqlResult result;
  if (!client->namespace_exists("test"))
    client->namespace_create("test");
  Namespace ns = client->namespace_open("test");
  client->hql_query(result, ns, "drop table if exists thrift_test");
  client->hql_query(result, ns, "create table thrift_test ( col )");
  client->hql_query(result, ns, "insert into thrift_test values"
            "('2008-11-11 11:11:11', 'k1', 'col', 'v1'), "
            "('2008-11-11 11:11:11', 'k2', 'col', 'v2'), "
            "('2008-11-11 11:11:11', 'k3', 'col', 'v3')");
  client->hql_query(result, ns, "select * from thrift_test");
  //out << result << std::endl;

  HqlResultAsArrays result_as_arrays;
  client->hql_query_as_arrays(result_as_arrays, ns, "select * from thrift_test");
  out << result_as_arrays << std::endl;
  for (int i = 0; i<result_as_arrays.cells.size(); i++)
  {
	  out << "result: \n";
	  for (int j = 0; j<result_as_arrays.cells[i].size(); j++)
	  {
		  out << " " << j << ": \'" <<  result_as_arrays.cells[i][j] << "\' ";
	  }
	  out << std::endl;
  }
  
  client->namespace_close(ns);
}

void test_scan(Thrift::Client *client, std::ostream &out) {
  ScanSpec ss;
  Namespace ns = client->namespace_open("test");

  Scanner s = client->open_scanner(ns, "pages", ss);
  std::vector<Hypertable::ThriftGen::Cell> cells;

  do {
    client->scanner_get_cells(cells, s);
    foreach_ht(const Hypertable::ThriftGen::Cell &cell, cells)
	{
	//	out << cell << std::endl;
       out << "row: " << cell.key.row << " col: " << cell.key.column_family 
			   << " value: " << cell.value << std::endl;
	}
  } while (cells.size());

  client->scanner_close(s);

 /* ss.cell_limit=1;
  ss.__isset.cell_limit = true;
  ss.row_regexp = "k";
  ss.__isset.row_regexp = true;
  ss.value_regexp = "^v[24].*";
  ss.__isset.value_regexp = true;
  ss.columns.push_back("col");
  ss.__isset.columns = true;

  s = client->open_scanner(ns, "thrift_test", ss);
  do {
    client->scanner_get_cells(cells, s);
    foreach_ht(const Hypertable::ThriftGen::Cell &cell, cells)
	{
      out << "key: " << cell.key << " value: " << cell.value << std::endl; 
	}
	out << std::endl;
  } while (cells.size());
  */ 
  client->namespace_close(ns);
}

// test for issue 484
void test_scan_keysonly(Thrift::Client *client, std::ostream &out) {
  ScanSpec ss;
  ss.keys_only=true;
  ss.__isset.keys_only = true;
  
  Namespace ns = client->namespace_open("test");

  Scanner s = client->open_scanner(ns, "thrift_test", ss);
  std::vector<Hypertable::ThriftGen::Cell> cells;
  
  do {
    client->scanner_get_cells(cells, s);
    foreach_ht(const Hypertable::ThriftGen::Cell &cell, cells)
      out << cell << std::endl;
  } while (cells.size());

  client->scanner_close(s);
}

void test_set(Thrift::Client *client) {
  std::vector<Hypertable::ThriftGen::Cell> cells;
  Namespace ns = client->namespace_open("test");

  Mutator m = client->mutator_open(ns, "thrift_test", 0, 0);
  cells.push_back(make_cell("k4", "col", "cell_limit", "v-ignore-this-when-cell_limit=1",
                            "2008-11-11 22:22:22"));
  cells.push_back(make_cell("k4", "col", 0, "v4", "2008-11-11 22:22:23"));
  cells.push_back(make_cell("k5", "col", 0, "v5", "2008-11-11 22:22:22"));
  cells.push_back(make_cell("k2", "col", 0, "v2a", "2008-11-11 22:22:22"));
  cells.push_back(make_cell("k3", "col", 0, "", "2008-11-11 22:22:22", 0,
                            ThriftGen::KeyFlag::DELETE_ROW));
  client->mutator_set_cells(m, cells);
  client->mutator_close(m);
  client->namespace_close(ns);
}

void test_schema(Thrift::Client *client, std::ostream &out) {
  Schema schema;
  Namespace ns = client->namespace_open("test");
  client->get_schema(schema, ns, "thrift_test");
  client->namespace_close(ns);

  std::map<std::string, AccessGroup>::iterator ag_it = schema.access_groups.begin();
  out << "thrift test access groups:" << std::endl;
  while (ag_it != schema.access_groups.end()) {
    out << "\t" << ag_it->first << std::endl;
    ++ag_it;
  }
  std::map<std::string, ColumnFamily>::iterator cf_it = schema.column_families.begin();
  out << "thrift test column families:" << std::endl;
  while (cf_it != schema.column_families.end()) {
    out << "\t" << cf_it->first << std::endl;
    ++cf_it;
  }
}

void test_put(Thrift::Client *client) {
  std::vector<Hypertable::ThriftGen::Cell> cells;
  MutateSpec mutate_spec;
  mutate_spec.appname = "test-cpp";
  mutate_spec.flush_interval = 1000;
  Namespace ns = client->namespace_open("test");

  cells.push_back(make_cell("put1", "col", 0, "v1", "2008-11-11 22:22:22"));
  cells.push_back(make_cell("put2", "col", 0, "this_will_be_deleted", "2008-11-11 22:22:22"));
  client->offer_cells(ns, "thrift_test", mutate_spec, cells);
  cells.clear();
  cells.push_back(make_cell("put1", "no_such_col", 0,
                "v1", "2008-11-11 22:22:22"));
  cells.push_back(make_cell("put2", "col", 0, "", "2008-11-11 22:22:23", 0,
                            ThriftGen::KeyFlag::DELETE_ROW));
  client->refresh_shared_mutator(ns, "thrift_test", mutate_spec);
  try {
    client->offer_cells(ns, "thrift_test", mutate_spec, cells);
  }
  catch (ClientException &e) {
    // ok, fall through
  }
  client->namespace_close(ns);
  sleep(2);
}

void test_async(Thrift::Client *client, std::ostream &out) {
  Namespace ns = client->namespace_open("test");
  String insert;
  int num_expected_results = 6;
  int num_results = 0;
  HqlResult hql_result;
  client->hql_query(hql_result, ns, "drop table if exists FruitColor");
  client->hql_query(hql_result, ns, "drop table if exists FruitLocation");
  client->hql_query(hql_result, ns, "drop table if exists FruitEnergy");
  client->hql_query(hql_result, ns, "create table FruitColor(data)");
  client->hql_query(hql_result, ns, "create table FruitLocation(data)");
  client->hql_query(hql_result, ns, "create table FruitEnergy(data)");


  // async writes
  {
    Future ff = client->future_open(0);
    MutatorAsync color_mutator       = client->async_mutator_open(ns, "FruitColor", ff, 0);
    MutatorAsync location_mutator    = client->async_mutator_open(ns, "FruitLocation", ff, 0);
    MutatorAsync energy_mutator      = client->async_mutator_open(ns, "FruitEnergy", ff, 0);
    std::vector<Hypertable::ThriftGen::Cell> color, location, energy;

    color.push_back(make_cell("apple", "data", 0, "red"));
    color.push_back(make_cell("kiwi", "data", 0, "brown"));
    color.push_back(make_cell("pomegranate", "data", 0, "pink"));

    location.push_back(make_cell("apple", "data", 0, "Western Asia"));
    location.push_back(make_cell("kiwi", "data", 0, "Southern China"));
    location.push_back(make_cell("pomegranate", "data", 0, "Iran"));

    energy.push_back(make_cell("apple", "data", 0 , "2.18kJ/g"));
    energy.push_back(make_cell("kiwi", "data", 0 , "0.61Cal/g"));
    energy.push_back(make_cell("pomegranate", "data", 0 , "0.53Cal/g"));

    client->async_mutator_set_cells(color_mutator, color);
    client->async_mutator_set_cells(energy_mutator, energy);
    client->async_mutator_set_cells(location_mutator, location);

    client->async_mutator_flush(color_mutator);
    client->async_mutator_flush(location_mutator);
    client->async_mutator_flush(energy_mutator);

    Result result;
    int num_results=0;
    while (true) {
      client->future_get_result(result, ff, 0);
      ++num_results;
      if (result.is_empty)
        break;
      if (result.is_scan == true) {
        out << "All results are expected to be from mutators" << std::endl;
        _exit(1);
      }
      if (result.is_error == true) {
        out << "Got unexpected error from mutator" << std::endl;
        _exit(1);
      }
      if (num_results > 3) {
        out << "Got unexpected number of results from mutator" << std::endl;
        _exit(1);
      }
      out << "Async flush successful" << std::endl;
    }

    if (client->future_has_outstanding(ff) || !client->future_is_empty(ff) || client->future_is_cancelled(ff)) {
      out << "Future should not have any outstanding operations or queued results or be cancelled" << std::endl;
      _exit(1);
    }

    client->future_cancel(ff);
    if (!client->future_is_cancelled(ff)) {
      out << "Future should be cancelled" << std::endl;
      _exit(1);
    }
    client->async_mutator_close(color_mutator);
    client->async_mutator_close(location_mutator);
    client->async_mutator_close(energy_mutator);
    client->future_close(ff);
  }

  RowInterval ri_apple;
  ri_apple.start_row = "apple";
  ri_apple.__isset.start_row = true;
  ri_apple.end_row= "apple";
  ri_apple.__isset.end_row = true;

  RowInterval ri_kiwi;
  ri_kiwi.start_row = "kiwi";
  ri_kiwi.__isset.start_row = true;
  ri_kiwi.end_row = "kiwi";
  ri_kiwi.__isset.end_row = true;

  RowInterval ri_pomegranate;
  ri_pomegranate.start_row = "pomegranate";
  ri_pomegranate.__isset.start_row = true;
  ri_pomegranate.end_row = "pomegranate";
  ri_pomegranate.__isset.end_row = true;

  ScanSpec ss;
  ss.row_intervals.push_back(ri_apple);
  ss.row_intervals.push_back(ri_kiwi);
  ss.row_intervals.push_back(ri_pomegranate);
  ss.__isset.row_intervals = true;

  Future ff = client->future_open(0);
  ScannerAsync color_scanner     = client->async_scanner_open(ns, "FruitColor", ff, ss);
  ScannerAsync location_scanner  = client->async_scanner_open(ns, "FruitLocation", ff, ss);
  ScannerAsync energy_scanner    = client->async_scanner_open(ns, "FruitEnergy", ff, ss);

  Result result;
  ResultSerialized result_serialized;
  ResultAsArrays result_as_arrays;

  while (true) {
    if (num_results<2) {
      client->future_get_result(result, ff, 0);
      if (result.is_empty)
        break;
      if (result.is_scan == false) {
        out << "All results are expected to be from scans" << std::endl;
        _exit(1);
      }
      if (result.is_error == true) {
        out << "Got unexpected error from async scan " << result.error_msg << std::endl;
        _exit(1);
      }

      if (result.id == color_scanner)
        out << "Got result from FruitColor: ";
      else if (result.id == location_scanner)
        out << "Got result from FruitLocation: ";
      else if (result.id == energy_scanner)
        out << "Got result from FruitEnergy: ;";
      else {
        out << "Got result from unknown scanner id " << result.id
            << " expecting one of " << color_scanner << ", " << location_scanner << ", "
            << energy_scanner << std::endl;
        _exit(1);
      }
      for (size_t ii=0; ii< result.cells.size(); ++ii) {
        out << result.cells[ii] << std::endl;
        num_results++;
      }
    }
    else if (num_results < 4) {
      client->future_get_result_as_arrays(result_as_arrays, ff, 0);
      if (result_as_arrays.is_empty)
        break;
      if (result_as_arrays.is_scan == false) {
        out << "All results are expected to be from scans" << std::endl;
        _exit(1);
      }
      if (result_as_arrays.is_error == true) {
        out << "Got unexpected error from async scan " << result_as_arrays.error_msg
            << std::endl;
        _exit(1);
      }

      if (result_as_arrays.id == color_scanner)
        out << "Got result_as_arrays from FruitColor: ";
      else if (result_as_arrays.id == location_scanner)
        out << "Got result_as_arrays from FruitLocation: ";
      else if (result_as_arrays.id == energy_scanner)
        out << "Got result_as_arrays from FruitEnergy: ;";
      else {
        out << "Got result_as_arrays from unknown scanner id " << result.id
            << " expecting one of " << color_scanner << ", " << location_scanner << ", "
            << energy_scanner << std::endl;
        _exit(1);
      }
      for (size_t ii=0; ii < result_as_arrays.cells.size(); ++ii) {
        out << "{" ;
        for (size_t jj=0; jj < result_as_arrays.cells[ii].size(); ++jj) {
          if (jj > 0)
            out << ", ";
          out << (result_as_arrays.cells[ii])[jj];
        }
        out << "}" << std::endl;
        num_results++;
      }
    }
    else if (num_results < 6){
      client->future_get_result_serialized(result_serialized, ff, 0);
      if (result_serialized.is_empty)
        break;
      if (result_serialized.is_scan == false) {
        out << "All results are expected to be from scans" << std::endl;
        _exit(1);
      }
      if (result_serialized.is_error == true) {
        out << "Got unexpected error from async scan " << result_serialized.error_msg
            << std::endl;
        _exit(1);
      }

      if (result_serialized.id == color_scanner)
        out << "Got result_serialized from FruitColor: ";
      else if (result_serialized.id == location_scanner)
        out << "Got result_serialized from FruitLocation: ";
      else if (result_serialized.id == energy_scanner)
        out << "Got result_serialized from FruitEnergy: ;";
      else {
        out << "Got result_serialized from unknown scanner id " << result_serialized.id
            << " expecting one of " << color_scanner << ", " << location_scanner << ", "
            << energy_scanner << std::endl;
        _exit(1);
      }
      SerializedCellsReader reader((void *)result_serialized.cells.c_str(),
                                   (uint32_t)result_serialized.cells.length());
      Hypertable::Cell hcell;
      while(reader.next()) {
        reader.get(hcell);
        out << hcell << std::endl;
        num_results++;
      }
    }
    else {
      client->future_cancel(ff);
      break;
    }
  }

  out << "Asynchronous scans finished" << std::endl;

  // -------------------------------------------------------------------
  out << "Testing ColumnPredicates" << std::endl;
  ScanSpec spec;
  ColumnPredicate cp;
  cp.column_family = "data";
  cp.__isset.column_family = true;
  cp.operation = ColumnPredicateOperation::PREFIX_MATCH;
  
  cp.__isset.operation = true;
  cp.value = "red";
  cp.__isset.value = true;
  spec.column_predicates.push_back(cp);
  spec.__isset.column_predicates = true;

  ScannerAsync cp_scanner = client->scanner_open(ns, "FruitColor", spec);
  while (true) {
    std::vector<Hypertable::ThriftGen::Cell> cells;
    client->next_cells(cells, cp_scanner);
    if (cells.empty())
      break;
    assert(cells.size()==1);
    foreach_ht(const Hypertable::ThriftGen::Cell &cell, cells) {
      out << cell << std::endl;
      assert(cell.key.row == "apple");
      assert(cell.value == "red");
    }
  }
  client->scanner_close(cp_scanner);
  // -------------------------------------------------------------------

  client->async_scanner_close(color_scanner);
  client->async_scanner_close(location_scanner);
  client->async_scanner_close(energy_scanner);
  client->future_close(ff);
  client->namespace_close(ns);
  if (num_results != num_expected_results) {
    out << "Expected " << num_expected_results << " received " << num_results << std::endl;
    _exit(1);
  }

}

void check_error(std::string expected, std::string &received,
                    std::ostream &out)
{
  if (received != expected) {
    out << "Expected: " << expected << "; received: " << received << std::endl;
    _exit(1);
  }
}

void test_error(Thrift::Client *client, std::ostream &out) {
  String s;

  client->error_get_text(s, 0);
  check_error("HYPERTABLE ok", s, out);
  client->error_get_text(s, 1);
  check_error("HYPERTABLE protocol error", s, out);
  client->error_get_text(s, 2);
  check_error("HYPERTABLE request truncated", s, out);
  client->error_get_text(s, 99999);
  check_error("ERROR NOT REGISTERED", s, out);
}