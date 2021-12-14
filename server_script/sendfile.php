<?php

require_once("_config.php");

$folder = $folder_ns . "/";
$newfolder = "";

if(!isset($_POST['action']))
{
	echo "POST_error\naction is empty\n\n";
	exit;
}

#Create a target directory if not exists
if(!file_exists($folder))
{
	mkdir($folder);
}

if($_POST['action'] != "oneday")
{
	echo "POST_error\naction is wrong\n\n";
	exit;
}


function get_file_extension($file_path)
{
	$basename = basename($file_path); //Getting filename
	if( strrpos($basename, '.') !== false )
	{
		//Check is file has a dot character
		//crop last part after dot character
		$file_extension = substr($basename, strrpos($basename, '.'));
	} else {
		//if dot is not found, set extension as empty
		$file_extension = "";
	}

	return $file_extension;
}

function extractFileName($filename)
{
    $p = strrpos($filename, '.');
    if($p > 0)
        return substr($filename, 0, $p);
    else
        return $filename;
}

//Convert Cyrilic into translit
function rus2translit($string)
{
    $converter = array(
        'а' => 'a',   'б' => 'b',   'в' => 'v',
        'г' => 'g',   'д' => 'd',   'е' => 'e',
        'ё' => 'e',   'ж' => 'zh',  'з' => 'z',
        'и' => 'i',   'й' => 'y',   'к' => 'k',
        'л' => 'l',   'м' => 'm',   'н' => 'n',
        'о' => 'o',   'п' => 'p',   'р' => 'r',
        'с' => 's',   'т' => 't',   'у' => 'u',
        'ф' => 'f',   'х' => 'h',   'ц' => 'c',
        'ч' => 'ch',  'ш' => 'sh',  'щ' => 'sch',
        'ь' => "-",   'ы' => 'y',   'ъ' => "-",
        'э' => 'e',   'ю' => 'yu',  'я' => 'ya',

        'А' => 'A',   'Б' => 'B',   'В' => 'V',
        'Г' => 'G',   'Д' => 'D',   'Е' => 'E',
        'Ё' => 'E',   'Ж' => 'Zh',  'З' => 'Z',
        'И' => 'I',   'Й' => 'Y',   'К' => 'K',
        'Л' => 'L',   'М' => 'M',   'Н' => 'N',
        'О' => 'O',   'П' => 'P',   'Р' => 'R',
        'С' => 'S',   'Т' => 'T',   'У' => 'U',
        'Ф' => 'F',   'Х' => 'H',   'Ц' => 'C',
        'Ч' => 'Ch',  'Ш' => 'Sh',  'Щ' => 'Sch',
        'Ь' => "-",   'Ы' => 'Y',   'Ъ' => "-",
        'Э' => 'E',   'Ю' => 'Yu',  'Я' => 'Ya',
	    ' ' => '_', "'" => "-", ":" => "-", "#" => "Sharp", "(" => "_", ")" => "_"
    );

    $str = strtr($string, $converter);
    $str = preg_replace('/[[:^print:]]/', '', $str);
    return $str;
}

$new_filename = rus2translit($_FILES["File1"]["name"]);

//Checking is file was been uploaded
if(is_uploaded_file($_FILES["File1"]["tmp_name"]))
{
	/***********************avoid uploading of the exploids****************************/
    if(preg_match('/\.(php|htm|html|js)$/i', $new_filename))
	{
		$new_filename .= ".txt";
	}

	if($new_filename == ".htaccess")
	{
		$new_filename = "my" . $new_filename . ".txt";
	}
	/**********************************************************************************/

	$new_filename = iconv('UTF-8', 'ASCII//TRANSLIT', $new_filename);

	if(file_exists($folder.$new_filename))
	{
		/***********Avoid overriding existing files************/
		$newfolder = rand()."/";
		while(file_exists($folder.$newfolder))
		{
			$newfolder = rand()."/";
		}
		mkdir($folder.$newfolder);
		$folder = $folder.$newfolder;
	}
    //If file successfully uploaded
    //move it from temp directory into target place
    move_uploaded_file($_FILES["File1"]["tmp_name"], $folder.$new_filename);
}
else
{
	echo("LOAD_ERROR\nFile not uploaded\n\n");
	exit;
}

chmod($folder.$new_filename, 0664);

if(!strstr($_SERVER['REMOTE_ADDR'], "172.16.10"))
{
	$source = "=====================================================\n"
	.date('Y F j  h:i')."   file Uploading  through CMD\n".
	"-----------------------------------------------------\n".
	$_FILES["File1"]["name"]."    ".
	$_FILES["File1"]["size"]." bytes \n".$_SERVER['REMOTE_ADDR']."\n\n";
	$Saved_File = fopen($log_filename, 'a+');
	fwrite($Saved_File, $source);
	fclose($Saved_File);
}

echo $url.$newfolder.$new_filename;
