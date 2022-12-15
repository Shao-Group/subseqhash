#!/usr/bin/env k8

/*
  Given two overlap detection results, compute their intersection or union.

  In the files, each line starts with the two labels of the related pair. The files are assumed to
  have been sorted by -k1,1n -k2,2n, and there is no repeat wrt the first two fields.

  Intersection is used for comparing the ground truth with another result. The third field of the ground
  truth file is the length of the overlap (on reference), where the third field of the other result
  would be the number of shared seeds. Both will be concatednated to the resulting file.

  Union is used for merging two results files from different random seeds. The third field of both files
  is the number of shared seeds, the sum will be stored in the resulting file.

  By: Ke@PSU
  Last edited: 10/21/2022
*/

function processOneLine(fin, buf){
    var result={
	has_s: false,
	s: null,
	p: null
    };
    if(fin.readline(buf) >= 0){
	result.has_s = true;
	result.s = buf.toString();
	//var p = s.split(" ").map(x => parseInt(x));
	result.p = result.s.split(" ").map(function(x){return parseInt(x)});
    }
    return result;
}

function main(args)
{
    if(args.length != 3){
	print("Usage: overlapFileOp.js <union|intersection> <file1> <file2>");
	exit(1);
    }

    var do_union = (args[0] == "union");
    var buf = new Bytes();
    var fin1 = new File(args[1]);
    var fin2 = new File(args[2]);


    var l1 = processOneLine(fin1, buf);
    var l2 = processOneLine(fin2, buf);
    
    while (l1.has_s && l2.has_s) {
	if(l1.p[0] > l2.p[0]){
	    if(do_union) print(l2.s);
	    l2 = processOneLine(fin2, buf);
	}else if(l1.p[0] < l2.p[0]){
	    if(do_union) print(l1.s);
	    l1 = processOneLine(fin1, buf);
	}else{//p1[0] == p2[0]
	    if(l1.p[1] > l2.p[1]){
		if(do_union) print(l2.s);
		l2 = processOneLine(fin2, buf);
	    }else if(l1.p[1] < l2.p[1]){
		if(do_union) print(l1.s);
		l1= processOneLine(fin1, buf);
	    }else{//p1[1] == p2[1]
		if(do_union){
		    print([l1.p[0], l1.p[1], l1.p[2] + l2.p[2]].join(" "));
		}else{
		    print([l1.p[0], l1.p[1], l1.p[2], l2.p[2]].join(" "));
		}
		l1 = processOneLine(fin1, buf);
		l2 = processOneLine(fin2, buf);
	    }
	}
    }

    if(do_union){
	while(l1.has_s){
	    print(l1.s);
	    l1 = processOneLine(fin1, buf);
	}
	while(l2.has_s){
	    print(l2.s);
	    l2 = processOneLine(fin2, buf);
	}
    }

    fin1.close();
    fin2.close();
    buf.destroy();
}

main(arguments);
