function get_files
{
    echo geo.xml
}

function po_for_file
{
    case "$1" in
       geo.xml)
           echo xml_mimetypes.po
       ;;
    esac
}

function tags_for_file
{
    case "$1" in
       geo.xml)
           echo comment
       ;;
    esac
}
