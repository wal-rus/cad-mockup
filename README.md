#Purpose

CadMockup is a simple set of tools mocking up functionality for a cad program

##Requirements

CMake 3.1+

C++11 compatible compiler (Tested on XCode 6)

##Usage

`cadquote <pathfile.json>`

##External Libraries

picojson - https://github.com/kazuho/picojson

##Notes
There's still plenty to be done in this project, and as a toy project I made some choices in the name of expediencey I probably wouldn't have for a real project.

Architecturally speaking, this is still a relatively simple project, but tries to follow a basic pattern of 'build a working version first, do semantic compression after to pull out useful abstractions'. I tend to like this approach since I've seen far too many projects where the abstraction layers hurt more than they help.

Some architectural choices of note:
 - Business info & logic around tooling is kept separate from the path logic
 - POD structs with external functions are generally preffered for simpler structures.
 - virtual inheratince was intentionally avoided since there are only 2 edge types with very different properties.


Possible Improvements:
 - Read the tooling info (max speed, padding, cost) from a json file as well.
 - Further semantic compression of the serialization code (only 2 passes)
 - Memoize the radius, arc lines, and arc angles for the ArcEdges
 - Improve command line parsing (boost program_options would work, but then i'd be using boost...)
 - Move all json parsing out of ToolPath. I put it there because it was expedient, but it was not a good choice architectually. ToolPaths may have alternative methods for construction, and should be able to be created purely from source
 - Move to a declarative json serialization system
 - Add unit tests of all independent classes (GTest would be a good choice)
 - Remove Vector2, replace with existing math library
 - Remove the raw pointers from ToolPath. They're fast and in this case safe, but in general it's not a great pattern and could cause big problems if people modified m_vertices.
