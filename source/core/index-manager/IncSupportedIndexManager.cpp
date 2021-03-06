#include "IncSupportedIndexManager.h"
#include "IIncSupportedIndex.h"
#include <document-manager/DocumentManager.h>

namespace sf1r
{

void IncSupportedIndexManager::addIndex(boost::shared_ptr<IIncSupportedIndex> index)
{
    if (index)
        inc_index_list_.push_back(index);
}

void IncSupportedIndexManager::flush()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->flush(true);
}

void IncSupportedIndexManager::optimize(bool wait)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->optimize(wait);
}

bool IncSupportedIndexManager::isRealTime()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        if (inc_index_list_[i]->isRealTime())
            return true;
    return false;
}

void IncSupportedIndexManager::setDocumentManager(boost::shared_ptr<DocumentManager>& documentManager)
{
    documentManager_ = documentManager;
}

void IncSupportedIndexManager::preBuildFromSCD(size_t total_filesize)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->preBuildFromSCD(total_filesize);
}

void IncSupportedIndexManager::postBuildFromSCD(time_t timestamp)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->postBuildFromSCD(timestamp);
}
void IncSupportedIndexManager::preMining()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->preMining();
}
void IncSupportedIndexManager::postMining()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->postMining();
}
void IncSupportedIndexManager::finishIndex()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->finishIndex();
}
void IncSupportedIndexManager::finishRebuild()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->finishRebuild();
}
void IncSupportedIndexManager::preProcessForAPI()
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->preProcessForAPI();
}
void IncSupportedIndexManager::postProcessForAPI()
{
    documentManager_->flush();
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->postProcessForAPI();
}

//all insertDocument is realtime ...
bool IncSupportedIndexManager::insertDocument(const Document& doc, time_t timestamp)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->insertDocument(doc, timestamp);
    return true;
}

bool IncSupportedIndexManager::updateDocument(const Document& olddoc, const Document& old_rtype_doc,
    const Document& newdoc, int updateType, time_t timestamp)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->updateDocument(olddoc, old_rtype_doc, newdoc, updateType, timestamp);
    return true;
}

void IncSupportedIndexManager::removeDocument(docid_t docid, time_t timestamp)
{
    for (size_t i = 0; i< inc_index_list_.size(); ++i)
        inc_index_list_[i]->removeDocument(docid, timestamp);
}

}
