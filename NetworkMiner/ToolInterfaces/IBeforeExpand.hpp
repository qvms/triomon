#pragma once

namespace NetworkMiner {
namespace ToolInterfaces {

class IBeforeExpand {
public:
    virtual ~IBeforeExpand() = default;
    virtual void BeforeExpand() = 0;
};

}
}
