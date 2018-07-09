'''
Convert a checkpoint file to a frozen grapth
(https://github.com/mrharicot/monodepth)

1. You can either download a pre-trained model or train a model from scratch

2. To make sure the frozen graph has the proper output name, do the following
  i. define the 'network output' in build_outputs() method in monodepth_model.py
    self.out = tf.expand_dims(self.disp1[:, :, :, 0], 3, name='output_depth')

NOTE: the code is not tested, because I have done some modifications in the original code. Should you have any problem, feel free to open an issue, I am happy to help.
'''

import tensorflow as tf
from monodepth import *

# Arguments
args = tf.app.flags
args.DEFINE_integer('batch_size', 2, 'The size of of a sample batch')
args.DEFINE_integer('img_height', 256, 'Image height')
args.DEFINE_integer('img_width', 512, 'Image width')
args.DEFINE_string('ckpt_file', '/path/to/monodepth/model/model_city2kitti', 'checkpoint file')
args.DEFINE_string('output_dir', '/path/to/frozen/graph/model', 'the folder that stores the graph')
args.DEFINE_string('graph', 'model_city2kitti.pb', 'the graph file')
arg = args.FLAGS

# Image placeholder
x = tf.placeholder(tf.float32, shape=[arg.batch_size, arg.img_height, arg.img_width, 3], name='input_image')

# Load model and get output (disparity)
model = monodepth(arg, x, 'vgg')
y = model.out

# add pb extension if not present
if not arg.graph.endswith(".pb"):
    arg.graph = arg.graph + ".pb"

# initialise the saver
saver = tf.train.Saver()

# SESSION
config = tf.ConfigProto(allow_soft_placement=True)
sess = tf.Session(config=config)

# Initialize variables
sess.run(tf.global_variables_initializer())
sess.run(tf.local_variables_initializer())

# SESSION
config = tf.ConfigProto(allow_soft_placement=True)
sess = tf.Session(config=config)

# restore all variables from checkpoint
saver.restore(sess, arg.ckpt_file)

# node that are required output nodes
output_node_names = ["model/output_depth"]

# We use a built-in TF helper to export variables to constants
output_graph_def = tf.graph_util.convert_variables_to_constants(
    sess,
    tf.get_default_graph().as_graph_def(),
    # The graph_def is used to retrieve the nodes
    output_node_names  # The output node names are used to select the useful nodes
)

# convert variables to constants
output_graph_def = tf.graph_util.remove_training_nodes(output_graph_def)

# Finally we serialize and dump the output graph to the filesystem
output_graph = arg.output_dir + '/' + arg.graph
with tf.gfile.GFile(output_graph, "wb") as f:
    f.write(output_graph_def.SerializeToString())

print("Frozen graph file {} created successfully".format(arg.graph))
