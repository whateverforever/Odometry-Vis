# pylint: skip-file

# background thread
def ComputeThread(frame_done_callback):
    while True:
        img_l, img_r = camera.GetImages()
        depth = stereo.ComputeDepth(img_l, img_r)
        rel_pose = odometry.ComputeCameraPos(img_l, img_last, depth)

        frame_done_callback(KFrame)

        img_last = img_l

# main thread
def main():
    myUI = Visualization()
    
    myData = ComputeThread([myUI](KFrame) {

        myUI.updateRGBLeft(*KFrame.left_img)
        myUI.updateDepth(*KFrame.depth_img)
        myUI.updateTrajectory(*KFrame.rel_pose)

    })

    myUI.runOnMainThread()
    # has to be last, since needs to be on main thread
