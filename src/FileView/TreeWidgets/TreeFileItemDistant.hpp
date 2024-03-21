//
// Created by Rahman on 31/03/2023.
//

#ifndef IRIDIUM_TREEFILEITEMDISTANT_HPP
#define IRIDIUM_TREEFILEITEMDISTANT_HPP

#include "TreeFileItem.hpp"
#include <boost/json.hpp>
#include <QDateTime>

class TreeFileItemDistant : public TreeFileItem
{
public:
    explicit TreeFileItemDistant(const RcloneFile &,TreeFileItem * = nullptr);

};


#endif //IRIDIUM_TREEFILEITEMDISTANT_HPP
