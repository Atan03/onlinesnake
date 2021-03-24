#ifndef _DOT_H_
#define _DOT_H_

#include<utility>
#include"../../common/commonUtils.h"
#include"../../protoFile/protoMsg.pb.h"

/**  
 * Class Dot represent the basic unit of a snake, private members are location
 * and symbol of this dot
 */
class Dot {
private:
    std::pair<int, int> locPr;
    char symbolShow;

public:
    Dot() = default;
    Dot(const std::pair<int, int> pr, char putSymbolShow): 
        locPr(pr), symbolShow(putSymbolShow) {}
    Dot(int putLocX, int putLocY, char putSymbolShow): 
        Dot({putLocX, putLocY}, putSymbolShow) {}
    Dot(const Dot &anotherDot): locPr(anotherDot.getLoc()), 
        symbolShow(anotherDot.getSymbol()) {}

    /* Get the dot's location */
    const std::pair<int, int> &getLoc() const {
        return locPr;
    }
    /* Get the symbol representing the dot */
    const char &getSymbol() const {
        return symbolShow;
    }

    /* Given direction and move range, the dot moves one step toward */
    void dotStep(const Step &putStep, int xMin, int xMax, int yMin, int yMax) {
        locPr.first = (locPr.first + putStep.xStep - (xMin + 1) + xMax - (xMin + 1)) 
            % (xMax - (xMin + 1)) + (xMin + 1);
        locPr.second = (locPr.second + putStep.yStep - yMin + yMax - yMin) 
            % (yMax - yMin) + yMin;
    }

    /* Compare whether two */
    bool operator==(const Dot &anotherDot) const {
        return anotherDot.getLoc() == locPr;
    }

    /* Add info into given protobuf */
    virtual void protoAdd(myProto::FrameMsg &frameMsg) const {
        auto pixelPtr = frameMsg.add_pixels();
        pixelPtr->set_x_loc(getLoc().first);
        pixelPtr->set_y_loc(getLoc().second);
        pixelPtr->set_symbol(getSymbol());
    }
};

#endif