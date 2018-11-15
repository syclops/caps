//
// Created by smaptas on 23.10.17.
//

#include "visitor.h"

#include <iostream>

GraphVisitor::~GraphVisitor()
{
  // Nothing to do here.
}

std::shared_ptr<GraphVisitor> GraphVisitor::clone() const
{
  return std::shared_ptr<GraphVisitor>{clone_raw()};
}

void GraphVisitor::process_node(const Node&)
{
  // Nothing to do here.
}

void GraphVisitor::process_label(const std::string&)
{
  // Nothing to do here.
}

bool GraphVisitor::should_proceed(const Node&, const Node&,
                                  const std::string&) const
{
  return true;
}

