# Convert a Tensorflow checkpoint file to a frozen graph
The Monodepth model is saved as a checkpoint file (.ckpt), we need to convert it to a graph file (.pb) so that the pre-trained model can be used

# Run the freeze_graph.py
python freeze_graph.py --encoder resnet --ckpt_file /path/to/trained/model --output_dir /path/to/output/folder

Note: 

* `--encoder resnet OR vgg`
* There is no extension (e.g., .ckpt) for the checkpoint file 
* change the file name of the output graph using `--graph output.pb`

# Download the pre-trained frozen graph
[VGG model](https://drive.google.com/open?id=1yzcndbigENP3kQg6Oioerwvkf_hTotZZ)

[Resnet50 model](https://drive.google.com/open?id=1SFd-FBGWwWHl1n6coIQV_EWhXUDvlWsk)

Note:
* VGG model: the pre-trained city2kitti model provided by Monodepth author
* Resnet50 model: city2kitti excluding odometry sequence 00-10



