#pragma once

#include "MurkaTypes.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"

#ifndef MURKA_MOSAIC_LAYOUT_DEF

#define MURKA_MOSAIC_LAYOUT_DEF
class MosaicLayout {
    ///
    
public:
    
    void restart() {
        thisFrameShapes.clear();
    }
    
    
    std::vector<MurkaShape> thisFrameShapes;
    
    inline bool upperThanAllFour(MurkaPoint input, MurkaPoint a, MurkaPoint b, MurkaPoint c, MurkaPoint d) {
        return ((input.y <= a.y) && (input.y <= b.y) && (input.y <= c.y) && (input.y <= d.y));
    }

    inline bool lowerThanAllFour(MurkaPoint input, MurkaPoint a, MurkaPoint b, MurkaPoint c, MurkaPoint d) {
        return ((input.y >= a.y) && (input.y >= b.y) && (input.y >= c.y) && (input.y >= d.y));
    }

    inline bool toTheLeftOfAllFour(MurkaPoint input, MurkaPoint a, MurkaPoint b, MurkaPoint c, MurkaPoint d) {
        return ((input.x <= a.x) && (input.x <= b.x) && (input.x <= c.x) && (input.x <= d.x));
    }

    inline bool toTheRightOfAllFour(MurkaPoint input, MurkaPoint a, MurkaPoint b, MurkaPoint c, MurkaPoint d) {
        return ((input.x >= a.x) && (input.x >= b.x) && (input.x >= c.x) && (input.x >= d.x));
    }

    inline MurkaPoint getIntersectionVector(MurkaShape a, MurkaShape b) {
        
        MurkaPoint leftTopPointA = MurkaPoint(a.position.x,
                                              a.position.y);
        MurkaPoint bottomRightPointA = MurkaPoint(a.position.x + a.size.x,
                                                  a.position.y + a.size.y);
        MurkaPoint bottomLeftPointA = MurkaPoint(a.position.x,
                                                 a.position.y + a.size.y);
        MurkaPoint rightTopPointA = MurkaPoint(a.position.x + a.size.x,
                                               a.position.y);
        
        MurkaPoint bottomRightPointB = MurkaPoint(b.position.x + b.size.x,
                                                  b.position.y + b.size.y);

        MurkaPoint leftTopPointB = MurkaPoint(b.position.x, b.position.y);

        MurkaPoint rightTopPointB = MurkaPoint(b.position.x + b.size.x, b.position.y);

        MurkaPoint bottomLeftPointB = MurkaPoint(b.position.x, b.position.y + b.size.y);

        if (b.inside(leftTopPointA, 0)) {
            // return the difference between bottom right of shape B and left top
            return bottomRightPointB - leftTopPointA;
        }

        if (b.inside(bottomRightPointA, 0)) {
            // return the difference between top left of shape B and bottom right
            return leftTopPointB - bottomRightPointA;
        }

        if (b.inside(bottomLeftPointA, 0)) {
            // return the difference between bottom right of shape B and left top
            return rightTopPointB - bottomLeftPointA;
        }

        if (b.inside(rightTopPointA, 0)) {
            // return the difference between top left of shape B and bottom right
            return bottomLeftPointB - rightTopPointA;
        }
        
        if ((toTheLeftOfAllFour(leftTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (toTheLeftOfAllFour(bottomLeftPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (toTheRightOfAllFour(rightTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (toTheRightOfAllFour(bottomRightPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB))) {
            
//            ofLog() << "OBERLAP TYPE Z";
            
            float xComponent = 0;
            if (abs(bottomLeftPointA.x - bottomRightPointB.x) <
                abs(bottomRightPointA.x - bottomLeftPointB.x)) {
                // moving it to the right
                return {bottomLeftPointA.x - bottomRightPointB.x, 0};
            } else return {bottomRightPointA.x - bottomLeftPointB.x, 0};

        }

        if ((upperThanAllFour(leftTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (upperThanAllFour(rightTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (lowerThanAllFour(bottomLeftPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB)) &&
            (lowerThanAllFour(bottomRightPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB))) {
            
            if ((toTheLeftOfAllFour(leftTopPointB, bottomRightPointA, leftTopPointA, rightTopPointA, bottomLeftPointA)) &&
                (toTheLeftOfAllFour(bottomLeftPointB, bottomRightPointA, leftTopPointA, rightTopPointA, bottomLeftPointA)) &&
                (toTheRightOfAllFour(bottomRightPointB, bottomRightPointA, leftTopPointA, rightTopPointA, bottomLeftPointA)) &&
                (toTheRightOfAllFour(rightTopPointB, bottomRightPointA, leftTopPointA, rightTopPointA, bottomLeftPointA))) {
//                ofLog() << "A overlaps B by y, B overlaps A by x..";
                
                float yComponent = 0;
                if ((abs((bottomLeftPointB - leftTopPointB).length()) <
                     abs((leftTopPointA - bottomLeftPointB).length()))) {
                    yComponent = bottomLeftPointB.y - leftTopPointB.y;
                } else {
                    yComponent = leftTopPointA.y - bottomLeftPointB.y;
                }
                
                return {0, yComponent};
            }
            
        }
        
        if (upperThanAllFour(leftTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB) &&
            lowerThanAllFour(bottomLeftPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB) &&
            toTheLeftOfAllFour(leftTopPointA, bottomRightPointB, bottomRightPointB, rightTopPointB, rightTopPointB) &&
            toTheRightOfAllFour(bottomRightPointA, bottomRightPointB, bottomRightPointB, rightTopPointB, rightTopPointB)) {
            
            return {bottomRightPointB.x - leftTopPointA.x, 0};
        }

        if (upperThanAllFour(leftTopPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB) &&
            lowerThanAllFour(bottomLeftPointA, bottomRightPointB, leftTopPointB, rightTopPointB, bottomLeftPointB) &&
            toTheRightOfAllFour(rightTopPointA, bottomLeftPointB, bottomLeftPointB, leftTopPointB, leftTopPointB) &&
            toTheLeftOfAllFour(leftTopPointA, bottomLeftPointB, bottomLeftPointB, leftTopPointB, leftTopPointB)) {
            
            return {bottomLeftPointB.x - rightTopPointA.x, 0};
        }

        return {0, 0};
    }
    
    
    // This functions searches through the collected rectangles and finds the one
    // with which the overlapping vector is the largest. It means you can add that
    // vector to the rectangle to move it away from the intersection, practically
    // either only X or only Y can be used
    
    inline MurkaPoint getMaxOverlappingVector(MurkaShape input) {
        bool badShape = false;
        float maxOverlap = 0;
        MurkaPoint maxOverlappingVector;
        for (auto i: thisFrameShapes) {
            auto intersection = getIntersectionVector(input, i);
            
            MurkaPoint biggestIntersection = intersection * float(1);

            const auto len = intersection.length();
            
            if (len > 0) {
                badShape = true;
                if (len > maxOverlap) {
                    maxOverlap = len;
                    maxOverlappingVector = biggestIntersection;
                }
            }
        }
        
        return maxOverlappingVector;
    }
    
    inline float overlapArea(MurkaShape s1, MurkaShape s2) {
     
        float left = max(s1.position.x, s2.position.x);
		float right = min(s1.position.x + s1.size.x, s2.position.x + s2.size.x);
		float bottom = min(s1.position.y + s1.size.y, s2.position.y + s2.size.y);
		float top = max(s1.position.y, s2.position.y);

        if ((left < right) && (top < bottom)) {
            return (right - left) * (bottom - top);
        } else return 0;
    }
    
    inline double overlapAreaOfOneToAll(MurkaShape shape, std::vector<MurkaShape>& others) {
        double sumOverlapArea = 0;
        for (auto &j: others) {
            sumOverlapArea += overlapArea(shape, j);
        }
        
        return sumOverlapArea;
    }
    
    MurkaShape outerBounds = {0, 0, 0, 0};
    
    // TODO: make it so if the shape gets out of bounds it's projected further away from the bound,
    // not merely moved away
    
    inline void fitToOuterBounds(MurkaPoint & position, MurkaShape inputShape) {
        if (outerBounds.size.length() > 0) {
            if (position.y < outerBounds.position.y) {
                position.y += outerBounds.position.y - position.y;
            }
            if (position.x < outerBounds.position.x) {
                position.x += outerBounds.position.x - position.x;
            }
            if ((position.y + inputShape.size.y) > (outerBounds.position.y + outerBounds.size.y)) {
                position.y += (outerBounds.position.y + outerBounds.size.y - (position.y + inputShape.size.y));
            }
            if ((position.x + inputShape.size.x) > (outerBounds.position.x + outerBounds.size.x)) {
                position.x += (outerBounds.position.x + outerBounds.size.x - (position.x + inputShape.size.x));
                
            }
        }
    }
    
#define ATTEMPTS_AT_EACH_STAGE 5
    
    vector<MurkaShape> compactnessShapes;

	float noise(int x, int y) {
		int n;

		n = x + y * 57;
		n = (n << 13) ^ n;
		return (1.0 - ((n * ((n * n * 15731) + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}

    MurkaShape fit(MurkaShape input, int depth = 25, int compactnessDepth = 10, int compactnessSteps = 3) {
        // 1. Search for shape options until we reach at aleast one 0 overlap option
        // if 1 cycle is not enough, try once more, up until the depth is reached
        
        bool foundZeroOverlapOption = false;
        std::vector<MurkaShape> validOptions;
        int currentDepth = depth;
        int stage = 1;
        
        // With each depth step we enlarge the search distance. It goes on until the suitable option found, or just goes until the max depth is reached, at which point we
        // go to the next step with all the collected options to select the less bad.
        while ((currentDepth > 0) && (!foundZeroOverlapOption)) {
            MurkaPoint thisStageAttempts[ATTEMPTS_AT_EACH_STAGE];
            
            for (int i = 0; i < ATTEMPTS_AT_EACH_STAGE; i++) {
                
                if ((currentDepth == depth) && (i == 0)) {
                    // first option is original
                    thisStageAttempts[i]  = input.position;
                } else {
                
					
                    thisStageAttempts[i] = MurkaPoint((noise(thisFrameShapes.size() + float(i / 5.), 0) * 2. - 1.) * input.size.length() * stage,
                                                      (noise(thisFrameShapes.size() + float(i / 5.) + 25, 0) * 2. - 1.) * input.size.length() * stage) + input.position;
                }
                    
                fitToOuterBounds(thisStageAttempts[i], input);
            }

            for (int optionIndex = 0; optionIndex < ATTEMPTS_AT_EACH_STAGE; optionIndex++) {
                    
                    MurkaShape offer = input;
                    offer.position = thisStageAttempts[optionIndex];
                    
                    offer.position += getMaxOverlappingVector(offer);
                    fitToOuterBounds(offer.position, input);
                    
                    auto thisOfferOverlapFactor = overlapAreaOfOneToAll(offer, thisFrameShapes);
                    
                    if (foundZeroOverlapOption) {
                        if (thisOfferOverlapFactor == 0) {
                            validOptions.push_back(offer);
                        }
                    } else { // didn't find zero yet...
                        if (thisOfferOverlapFactor == 0) { // this was the first zero overlap!
                            validOptions.clear();
                            validOptions.push_back(offer);
                            
                            foundZeroOverlapOption = true;
                        } else { // it wasn't zero but we can use it if no zeroes found
                            validOptions.push_back(offer);
                        }
                    }
            }
            
            stage++;
            currentDepth--;
        }
        
        // 2. Out of those 0 zero overlap options, pick the closest to beginning
        
        MurkaShape step2Option;
        if (validOptions.size() > 0) {
            
            sort(validOptions.begin(), validOptions.end(),
                [&](const MurkaShape & a, const MurkaShape & b) -> bool
            {
                return (a.position - input.position).length() <
                       (b.position - input.position).length();
            });
            
            step2Option = validOptions[0];
//            return validOptions[0];
        } else {
            // What happened?
            ofLog() << "  WARNING!";
            thisFrameShapes.push_back(input);
            return input;
        }
        
        // 3. Try to move it even closer to beginning by projecting towards beginning and trying a couple more opts
        
        MurkaShape step3Option = step2Option;
        
//        compactnessShapes.resize(compactnessDepth * 2);
        

        for (int c = 0; c < 3; c++) {
            MurkaShape initialOffer = step3Option;
            
            MurkaPoint compactnessVector = initialOffer.position - input.position;
            double compactnessStep = 1. / double(compactnessDepth);
            
            for (int i = 0; i < compactnessDepth; i++) {
                MurkaShape compactnessOfferX = initialOffer;
                compactnessOfferX.position.x -= double(i) * compactnessStep * compactnessVector.x;

                MurkaShape compactnessOfferY = initialOffer;
                compactnessOfferY.position.y -= double(i) * compactnessStep * compactnessVector.y;
                
                if (overlapAreaOfOneToAll(compactnessOfferX, thisFrameShapes) == 0) step3Option = compactnessOfferX;
                if (overlapAreaOfOneToAll(compactnessOfferY, thisFrameShapes) == 0) step3Option = compactnessOfferY;
            }
        }

        thisFrameShapes.push_back(step3Option);
        return step3Option;
    }
    

};

#endif
