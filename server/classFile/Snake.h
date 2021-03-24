#ifndef _SNAKE_H
#define _SNAKE_H

#include<deque>
#include"Dot.h"
#include"../../common/commonUtils.h"
#include"../../protoFile/protoMsg.pb.h"

/**  
 * Class Snake inherits class Dot, with a Dot in it and three other 
 * private members: snakeBody is a deque indicates body dots, action2Exec
 * is the action list receied from player which are to be executed, action
 * is the action which is about to direct the snake's step
 */
class Snake: public Dot {
private:
    std::deque<Dot> snakeBody;
    std::deque<Action> action2Exec;
    Action action;

    void setActionFromList(Action putAction) {
        if((action == Action::kUp && putAction != Action::kDown)
            || (action == Action::kDown && putAction != Action::kUp)
            || (action == Action::kLeft && putAction != Action::kRight)
            || (action == Action::kRight && putAction != Action::kLeft)
            ) {
            //snake can never directly turn around
            action = putAction;
        }
    }
    Action getAction() {
        if(!action2Exec.empty()) {
            Action actionToRtn = action2Exec.front();
            action2Exec.pop_front();
            return actionToRtn;
        }
        else {
            //if there's no new action set, keep old direction
            return action;
        }
    }

public:
    Snake() = default;
    Snake(const std::pair<int, int> pr, const char symbol, Action putAction = Action::kLeft): 
        Dot(pr, symbol), action(Action::kLeft) {}
    Snake(const std::pair<int, int> pr, Action putAction = Action::kLeft): 
        Snake(pr, 'O', putAction) {}

    /* Put snake head into body, and generate another head with stored action,
       then update new action */
    void stepForward(int xMin, int xMax, int yMin, int yMax) {
        snakeBody.emplace_front(getLoc(), tolower(getSymbol()));
        
        auto iter = stepMap.find(action);
        if(iter != stepMap.end()) {
            dotStep(iter->second, xMin, xMax, yMin, yMax);
        }
        else {
            throw std::runtime_error("Illegal action received: " + 
                std::to_string(static_cast<int>(action)));
        }

        setActionFromList(getAction());
    }
    /* Simply put action into action list */
    void setActionToList(Action putAction) {
        action2Exec.push_back(putAction);
    }

    /* Given a pair indicates loc, return whether it is collided with snake's body */    
    bool collideLoc(const std::pair<int, int> &pr) const {
        for(auto &bodyDot : snakeBody) {
            if(bodyDot.getLoc() == pr) {
                return true;
            }
        }
        return false;
    }
    
    /* Simply cut the tail dot of snake */
    void tailCut() {
        snakeBody.pop_back();
    }

    /* Add snake body and head's info into given protobuf */
    void protoAdd(myProto::FrameMsg &frameMsg) const override{
        for(auto &dot : snakeBody) {
            dot.protoAdd(frameMsg);
        }
        Dot::protoAdd(frameMsg);
    }
};

#endif