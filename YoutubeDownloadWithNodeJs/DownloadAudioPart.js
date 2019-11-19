const path = require('path');
const fs   = require('fs');
const ytdl = require('..');
const matroska = require('matroska');

const url = 'https://www.youtube.com/watch?v=CehAKQL463M';

const FileName = 'audio_header.webm';
const output = path.resolve(__dirname, FileName);
const SecondsToDownload = 30; // this should be a multiply of 10 seconds

DownloadHeader(0);

function DownloadHeader(HeaderSize)
{
	//a 4 minute audio had 800 bytes header. Downloading 10k should be more than enough to get the cue list
	if(HeaderSize <= 0)
		HeaderSize = 10000;
	const video = ytdl(url, { range: { start: 0, end: HeaderSize }, filter: 'audioonly', quality: 'highestaudio' });
	video.pipe(fs.createWriteStream(output));
	video.on('end', () => {
	  ParseHeader();
	});
}

function ParseHeader()
{
	var decoder = new matroska.Decoder();
	decoder.parse(output, function ParseHeaderCallback(error, document){
		if (error) 
		{
			//console.error(error);
			return;
		}
		//there has got to be a better way to parse this
		var FormatedHeader = document.print();
		//console.log("Full header : ", FormatedHeader);
		var AudioLen = GetStrValFromDoc("Duration  f[4]=",FormatedHeader);
		console.log("Audio has length in ms : ", AudioLen);
		
		//this should be about 10 seconds length
		var OneCueLen = GetStrValFromDoc("CueTime  u[2]=",FormatedHeader);
		console.log("Cue has length in ms : ", OneCueLen);
		if(OneCueLen<1000)
			OneCueLen = 10001;
		
		var SkipCueCount = parseInt(AudioLen / OneCueLen / 2);
		var DownloadCount = parseInt((30000 + OneCueLen / 2 ) / OneCueLen);
		console.log("Number of cues to download :",DownloadCount);
		
		var Parts = FormatedHeader.split("CueClusterPosition  u[3]=");
		var CueCount = 0;
		var DownloadStarAt = 0;
		var DownloadEndAt = 0;
		var LastCue = 0;
		for(let val of Parts)
		{
			var subpart = val.split("\n");
			var CueIndex = subpart[0];
			if(CueCount == SkipCueCount)
			{
				console.log("Download from :",CueIndex);
				DownloadStarAt = CueIndex;
			}
			else if(CueCount == SkipCueCount + DownloadCount)
			{
				console.log("Download to :",CueIndex);
				DownloadEndAt = CueIndex;
			}
			CueCount++;
			LastCue = CueIndex;
		}
		if(DownloadEndAt == 0)
			DownloadEndAt = LastCue;
		var HeaderSize = GetHeaderSize(FormatedHeader);
		console.log("Header size in bytes :",HeaderSize);
		
		GenHeaderAndChunks(HeaderSize, DownloadStarAt, DownloadEndAt);
	});
}

function GetStrValFromDoc(key, doc)
{
	var parts = doc.split(key);
	var part = parts[1].split("\n")
	return part[0];
}

function GetHeaderSize(doc)
{
	var Parts = doc.split("\n");
	for(let val of Parts)
		if(val.indexOf("* Cluster  children") > 0 )
		{
			var subparts = val.split("#");
			return subparts[0];
		}
	return "";
}

function GenHeaderAndChunks(HeaderSize,ChunksStart,ChunksEnd)
{
	/*
	fs.open(output, 'r', function(status, fd) {
        if (status) {
            console.log(status.message);
            return;
        }
        var buffer = new Buffer(HeaderSize);
        fs.read(fd, buffer, 0, HeaderSize, 0, function(err, num)
		{
			console.log("read bytes",num);
		});
		/*fs.open("out.mkv","w",function(status, fd) {
			if (status) {
				console.log(status.message);
				return;
			}
			fs.write(fd, buffer,'binary', function(err){});
		});*/
   // });
//	const video = ytdl(url, { range: { start: 0, end: HeaderSize }, filter: 'audioonly', quality: 'highestaudio' });
//	video.pipe(fs.createWriteStream(output));

	const fd = fs.openSync(output, 'r+');
	fs.ftruncate(fd, parseInt(HeaderSize), (err) => {
//	  assert.ifError(err);
	});
	//fs.close(fd);
	
	const video2 = ytdl(url, { range: { start: ChunksStart, end: ChunksEnd }, filter: 'audioonly', quality: 'highestaudio' });
	video2.pipe(fs.createWriteStream(output, { flags: 'a' }));
}