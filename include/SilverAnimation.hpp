#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <vector>
#include <string>
#include <memory>

class Animation {
public:
    std::vector<std::string> animation;
    void LoadAnimationFromFile(const std::string filename);
    double fps;
    int transition;
    bool immediateTransition;
};


class AnimationManager : public Component {
public:
    AnimationManager() = default;
    AnimationManager(Animation* anim) : playing(anim), nextUp(nullptr), currentFrame(0) {}
    AnimationManager(const AnimationManager& other) { *this = other; }
    
    AnimationManager& operator=(const AnimationManager& other) {
        if (this != &other) {
            playing = other.playing;
            nextUp = other.nextUp;
            currentFrame = other.currentFrame;
        }
        return *this;
    }

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<AnimationManager>(*this);
    }
    
    void SwitchAnimation(Animation* anim);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    
    void Update(float deltaTime) {
      static double elapsedTime = 0.0f;
      double interval = 1000 / playing->fps;
      elapsedTime += DeltaTime();
      if(interval > elapsedTime) elapsedTime = 0.0f;
      else return;
      
      SpriteRenderer* spriteRenderer = parent->GetComponent<SpriteRenderer>();
      if(spriteRenderer != nullptr) {
        spriteRenderer->setShape(playing->animation[currentFrame]);
        
        if(nextUp != nullptr && (currentFrame==playing->transition || playing->immediateTransition)) {
          playing = nextUp;
          nextUp = nullptr;
          currentFrame=0;
        } 
        
        currentFrame++;
        currentFrame%=playing->animation.size();
      }
    }


private:
    Animation* playing = nullptr;
    Animation* nextUp = nullptr;
    int currentFrame = 0;
};

#endif // ANIMATION_MANAGER_H
