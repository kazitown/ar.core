#include "ARMarkerless.hpp"

ARMarkerless::ARMarkerless() {
    recognitionInstance = std::make_unique<DetectMarkerless>();
    scale = 1.f;
}

std::vector<QueryItem> ARMarkerless::process(const cv::Mat &frame) {
    if (frame.empty())
        return std::vector<QueryItem>{};

    // make query mat
    cv::cvtColor(frame, queryMat, cv::COLOR_BGR2GRAY);
    if (!querySize.empty())
        cv::resize(queryMat, queryMat, querySize);

    // we have two processes here: detection and tracking
    // also for multi-tracking purposes they are should be in separate threads
    // *-------------*                *-------------*
    // |             |  [detected]    |             |
    // |   detect    |   -------->    |  tracking   | ----> [result]
    // |             |                |             |
    // *_____________*                *_____________*
    //
    // detect process (DP) trying detect object on each passed frame:
    //  for each trackable object:
    //      sets status:
    //          detected => start tracking on it object
    //          _ => continue
    //
    // tracking process (TP) trying track object on each frame:
    //  wait for detected objects:
    //      for each detected object:
    //          object.detected => object.tracked = true
    //          object.tracked => object.pose = calcNewPose()
    //          object.lost => continue
    //      return objects{detected && tracked}.pose

    std::vector<QueryItem> recognizedItems, // detected objects
            result;         // tracked objects

    /* detection */

    // masking: make already detected regions not-detectable (maskedMat)
    // unmasked image (queryMat) for tracking stage

    if (trackingItems.empty()) {
        recognizedItems = recognitionInstance->queryImage(queryMat);
    } else {
        // mask
        queryMat.copyTo(maskedMat);
        for (const auto &item: trackingItems)
            maskedMat |= CvUtils::createMask(maskedMat.size(), item.first.objPose);
        recognizedItems = recognitionInstance->queryImage(maskedMat);
    }

    // adds newly detected objects
    for (const auto &item: recognizedItems) trackingItems.emplace_back(item, std::make_unique<Tracker>());

    /* tracking */
    for (auto it = trackingItems.begin(); it != trackingItems.end();) {

        auto item = &*it;
        bool status = false;
        auto tracking = &*item->second;
        switch (item->first.status) {
            case DETECTED: // start tracking
                item->first.status = TRACKED;
                // create tracking instance for item
                tracking->start(this->queryMat, item->first.objPose);
                status = true;
                break;
            case TRACKED:   // keep tracking
                status = tracking->keepTracking(this->queryMat);
                // update pose
                item->first.objPose = tracking->objectPosition;
                break;
            case LOST:
                break;
            default:
                throw std::runtime_error("Illegal state of trackable item");
        }

        if (!status) {
            // lost => remove
            item->first.status = LOST;
        } else if (item->first.status == TRACKED) {
            // pose scaling
            QueryItem r = item->first;
            if (!querySize.empty())
                r.objPose = CvUtils::scalePoints(item->first.objPose, scale);
            result.push_back(r);
        }

        // remove lost and go next
        if (it->first.status == LOST) {
            it = trackingItems.erase(it);
        } else
            ++it;

    }

    return result;
}

int ARMarkerless::addAll(const std::vector<cv::Mat> &imgs) {
    for (const auto &img: imgs) {
        recognitionInstance->addTrackImage(img);
    }
    return 0;
}

int ARMarkerless::add(const cv::Mat &img) {
    recognitionInstance->addTrackImage(img);
    return 0;
}

int ARMarkerless::add(const cv::Mat &descriptors, const std::vector<cv::KeyPoint> &keyPoints,
                      const cv::Size &size) {
    dynamic_cast<DetectMarkerless &>(*recognitionInstance).addTrackImage(descriptors, keyPoints, size);
    return 0;
}

void ARMarkerless::init(const cv::Size &frameSize, const int maxSize) {
    int frameMaxSize = std::max(frameSize.width, frameSize.height);
    scale = 1;
    while ((frameMaxSize / scale) > maxSize) {
        scale *= 2;
    }
    querySize = cv::Size(frameSize.width / scale, frameSize.height / scale);
}


ARMarkerless::~ARMarkerless() = default;



