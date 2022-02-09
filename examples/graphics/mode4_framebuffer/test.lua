-- Test that runs a program for 60 frames, takes a screenshot, presses the START
-- button to enable background wraparound and  to move and rotate the
-- background, and takes another screenshot.

run_frames_and_pause(60)
screenshot("screenshot-1.png")
keys_hold("START")

run_frames_and_pause(5)
screenshot("screenshot-2.png")

exit()

return 0
