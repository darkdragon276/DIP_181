void config_video ( Mat& frame )
{
  	const string file_name = "";
  	int codec = CV_FOURCC('M','J','P','G');
  	double fps = 25;
  	Size frame_size = img.size();
  	bool is_Color = true;
  	VideoWriter out_capture( file_name, codec, fps, frame_size	, is_Color);
}
void write_video( Mat& frame)
{
	while (true)
  	{
	    if(frame.empty())
	        break;
	    out_capture.write( frame );
  	}
}