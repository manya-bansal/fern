#ifndef PIPELINE_CORE_NODES
#define PIPELINE_CORE_NODES

#include "dependency_lang/dep_lang.h"
#include "dependency_resolver/pipeline_abstract_node.h"

namespace fern {

struct QueryNode : PipelineGraphNode {};

struct AllocateNode : PipelineGraphNode {};

struct ComputeNode : PipelineGraphNode {};

struct InsertNode : PipelineGraphNode {};

struct FreeNode : PipelineGraphNode {};

struct PipelineNode : PipelineGraphNode {};

} // namespace fern

#endif
