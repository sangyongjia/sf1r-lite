#include "PriceHistory.h"

#include <libcassandra/cassandra.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;
using namespace libcassandra;
using namespace org::apache::cassandra;

namespace sf1r {

const ColumnFamilyBase::ColumnType PriceHistory::column_type = ColumnFamilyBase::NORMAL;

const string PriceHistory::cf_name("PriceHistory");

const string PriceHistory::cf_column_type;

const string PriceHistory::cf_comparator_type("LongType");

const string PriceHistory::cf_sub_comparator_type;

const string PriceHistory::cf_comment(
    "This column family stores recent two years price history for each product.\n"
    "Schema:\n\n"
    "    column family PriceHistory = list of {\n"
    "        key \"product docid\" : list of {\n"
    "            name \"index timestamp\" : value \"product price\"\n"
    "        }\n"
    "    }");

const double PriceHistory::cf_row_cache_size(0);

const double PriceHistory::cf_key_cache_size(0);

const double PriceHistory::cf_read_repair_chance(0);

const vector<ColumnDef> PriceHistory::cf_column_metadata;

const int32_t PriceHistory::cf_gc_grace_seconds(0);

const string PriceHistory::cf_default_validation_class;

const int32_t PriceHistory::cf_id(0);

const int32_t PriceHistory::cf_min_compaction_threshold(0);

const int32_t PriceHistory::cf_max_compaction_threshold(0);

const int32_t PriceHistory::cf_row_cache_save_period_in_seconds(0);

const int32_t PriceHistory::cf_key_cache_save_period_in_seconds(0);

const int8_t PriceHistory::cf_replicate_on_write(-1);

const double PriceHistory::cf_merge_shards_chance(0);

const string PriceHistory::cf_key_validation_class("AsciiType");

const string PriceHistory::cf_row_cache_provider("SerializingCacheProvider");

const string PriceHistory::cf_key_alias;

const string PriceHistory::cf_compaction_strategy("LeveledCompactionStrategy");

const map<string, string> PriceHistory::cf_compaction_strategy_options;

const int32_t PriceHistory::cf_row_cache_keys_to_save(0);

const map<string, string> PriceHistory::cf_compression_options;

PriceHistory::PriceHistory(const string& docId)
    : ColumnFamilyBase()
    , docId_(docId)
    , priceHistoryPresent_(false)
{}

PriceHistory::~PriceHistory()
{}

const string& PriceHistory::getKey() const
{
    return docId_;
}

bool PriceHistory::getMultiSlice(
        map<string, PriceHistory>& row_map,
        const vector<string>& key_list,
        const string& start,
        const string& finish)
{
    if (!CassandraConnection::instance().isEnabled()) return false;
    try
    {
        ColumnParent col_parent;
        col_parent.__set_column_family(cf_name);

        SlicePredicate pred;
        //pred.slice_range.__set_count(numeric_limits<int32_t>::max());
        pred.slice_range.__set_start(start);
        pred.slice_range.__set_finish(finish);

        map<string, vector<ColumnOrSuperColumn> > raw_column_map;
        CassandraConnection::instance().getCassandraClient()->getMultiSlice(
                raw_column_map,
                key_list,
                col_parent,
                pred);
        if (raw_column_map.empty()) return true;

        for (map<string, vector<ColumnOrSuperColumn> >::const_iterator mit = raw_column_map.begin();
                mit != raw_column_map.end(); ++mit)
        {
            row_map[mit->first] = PriceHistory(mit->first);
            PriceHistory& price_history = row_map[mit->first];
            for (vector<ColumnOrSuperColumn>::const_iterator vit = mit->second.begin();
                    vit != mit->second.end(); ++vit)
            {
                price_history.insert(vit->column.name, vit->column.value);
            }
        }
    }
    catch (const InvalidRequestException &ire)
    {
        cout << ire.why << endl;
        return false;
    }
    return true;
}

bool PriceHistory::getMultiCount(
        map<string, int32_t>& count_map,
        const vector<string>& key_list,
        const string& start,
        const string& finish)
{
    if (!CassandraConnection::instance().isEnabled()) return false;
    try
    {
        ColumnParent col_parent;
        col_parent.__set_column_family(cf_name);

        SlicePredicate pred;
        //pred.slice_range.__set_count(numeric_limits<int32_t>::max());
        pred.slice_range.__set_start(start);
        pred.slice_range.__set_finish(finish);

        CassandraConnection::instance().getCassandraClient()->getMultiCount(
                count_map,
                key_list,
                col_parent,
                pred);
    }
    catch (const InvalidRequestException &ire)
    {
        cout << ire.why << endl;
        return false;
    }
    return true;
}

bool PriceHistory::updateRow() const
{
    if (!CassandraConnection::instance().isEnabled() || docId_.empty()) return false;
    if (!priceHistoryPresent_) return true;
    try
    {
        for (PriceHistoryType::const_iterator it = priceHistory_.begin();
                it != priceHistory_.end(); ++it)
        {
            CassandraConnection::instance().getCassandraClient()->insertColumn(
                    toBytes(it->second),
                    docId_,
                    cf_name,
                    toBytes(it->first),
                    createTimeStamp(),
                    63072000); // Keep the price history for two years at most
        }
    }
    catch (const InvalidRequestException &ire)
    {
        cout << ire.why << endl;
        return false;
    }
    return true;
}

bool PriceHistory::insert(const string& name, const string& value)
{
    clear();
    if (value.length() != sizeof(ProductPrice))
    {
        cerr << "Bad insert!" << endl;
        return false;
    }
    priceHistory_[fromBytes<time_t>(name)] = fromBytes<ProductPrice>(value);
    return true;
}

void PriceHistory::insert(time_t timestamp, ProductPrice price)
{
    clear();
    priceHistory_[timestamp] = price;
}

void PriceHistory::resetKey(const string& newDocId)
{
    if (newDocId.empty())
    {
        docId_.clear();
        priceHistoryPresent_ = false;
    }
    else
        docId_.assign(newDocId);
}

void PriceHistory::clear()
{
    if (!priceHistoryPresent_)
    {
        priceHistory_.clear();
        priceHistoryPresent_ = true;
    }
}

}
