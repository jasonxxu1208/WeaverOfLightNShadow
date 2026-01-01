# Weaver of Light and Shadow
**Unreal Engine 5 | C++ & Blueprint Gameplay Systems | First person puzzle action**

**Demo:** https://youtu.be/5cR2NjdWtfA 
## Project Overview
Weaver of Light & Shadow is a first-person puzzle-action game developed in Unreal Engine 5 using C++ and Blueprints, where light functions as both a traversal mechanic and a combat resource. Players wield a magical wand to interact with torches, cast powerful light spells, and defeat enemies while navigating environments shaped by illumination.

The project focuses on systems-driven gameplay, emphasizing real-time light evaluation, modular spell logic, and state-based AI behavior.

## Core Gameplay Mechannics
- **Wand-Based Spell System**
  - Toggle torches on/off without consuming resoources
  - Attack enemies using limited charges
  - Cast Lumos, a strong directional light that affects the environment
- **Light-Activated Shadow Bridge**
  - Platforms dynamically appear and disappear based on real-time light intensity
  - Evaluates multiple light sources (torches and spells) using custom falloff models
- **Enemy AI**
  - Patrol and chase states based on player proximity
  - Collide with the player to eliminate
  - Custom movement and gravity handling
- **Resource Management**
  - Limited spell charges displayed via an on-screen UI
  - Encourages decision-making on different paths

 ## Technical Highlights
- Engineered a light-gated ShadowBridge system in C++, computing gameplay-driven light intensity using distance and angular falloff to dynamically toggle bridge visibility and collision, making light a core traversal mechanic
- Implemented a modular Ghost AI state machine in C++ with patrol and chase behaviors driven by player distance, including deterministic instant-kill logic via player-only overlap filtering
- Designed a wand interaction system using UE5 Enhanced Input, decoupling input actions from gameplay logic through default mapping contexts to ensure consistent behavior across levels
- Developed a reusable rendering pipeline using a parameterized Master Material exposing emissive, opacity, and roughness controls, enabling rapid visual iteration with minimal shader permutations
- Built robust player and enemy state management incorporating gravity simulation, surface tagging, and KillZ handling, ensuring reliable death, respawn, and level transition behavior
- Constructed a modular environment architecture using Blueprint-based prefabs to standardize grid alignment and promote asset reuse across levels

## Project Structure
Source/WeaverOfLightNShadow/MyContent/

  WeaverOfLightNShadowCharacter(Player & Input handle, modified based on UE5 generated character class)
  
  MyWand (Spells & Charges)
  
  Torch (World Interaction)
  
  MyGhost (Enemy AI)
  
  MyShadowBridge (Light-Driven Platform)
  
Content/

  Audio(SFX)
  
  Blueprints(BP classes for AActor)
  
  FirstPerson(Character BP)
  
  Level(Level design)
  
  Materials(Master Material & MI)
  
  UI(Gamplay UI)
  
