#include "WikiReaver.h"
#include "Sauron/Heistron/Heist.h"
#include <Pinecone/Extension/Util/Net/lib_http.h>
#include <Sauron/Utils/IndexSniffer/ParadigmSniffer.h>
#include <shlwapi.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLParser.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLDocument.h>
#include <Pinecone/Framework/Util/DOM/HTML/Archetype/HTMLElement.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLNodeList.h>
#include <Sauron/System/defined.inc.h>
#include <Pinecone/Framework/Unit/Set/HashSet.h>
#include <Pinecone/Framework/Util/CSS/DOMTreeSelector.h>
#include <Pinecone/Framework/Util/Charset/Stereotype/PineCharsets.h>
#include <Sauron/Utils/NonaHelper.h>
#include <Pinecone/Framework/Util/Charset/StringCoding.h>
#include <Pinecone/Framework/Util/SQL/SQLString.h>
#include <Pinecone/Framework/Util/Protocol/HTTP/HTTPURLParser.h>
#include <Pinecone/Framework/Util/SQL/MappedSQLSplicer.h>
#include <Pinecone/Framework/Util/Charset/CodingProvider/UnicodeTransformat.h>

namespace Sauron {
    void WikiReaver::validateSpoil( const std::string &sz ) const {
        if( sz.length() < this->mFailureConf.nFailedFileSize ) {  // 1822
            throw Pinecone::IllegalStateException( "CompromisedFilesSize[1822 ERROR]." );
        }
        if( !Pinecone::StdString::endsWith( sz, "</html>" ) ) {
            throw Pinecone::IllegalStateException( "NetworkError::IncompleteFiles." );
        }
    }

    void WikiReaver::embezzle_index() const {
        this->mpParentHeistum->trace( { "[InitializeIndexJsonify] FileTasks:", this->mnLastStalkerId } );

        std::string& szIndexPath = this->mpParentHeistum->mszIndexPath;
        size_t nIndexCount = 0;
        Pinecone::Listron jIndex;
        for ( int i = 0; i < this->mnLastStalkerId + 1; ++i ) {
            std::string szFN   = szIndexPath + std::string( "index_" ).append( Pinecone::StdString::toString(i) ).append( ".html" ) ;
            std::ifstream ifs ( szFN, std::ios::binary );
            std::string szHtml = Pinecone::FileUtils::readAll( ifs );

            Pinecone::HTMLDocument document;
            Pinecone::HTMLParser   htmlParser ( szHtml );
            htmlParser.parse( &document );
            Pinecone::HTMLNodeList nodes = document.getElementsByClassName( "mw-allpages-chunk" );

            if( nodes.empty() || nodes.size() > 1 ) {
                this->mpParentHeistum->trace( { "[Notice: EmptyHtml]", i, nodes.size() } );
            }
            else {
                Pinecone::HTMLNode* lpChunk = nodes[ 0 ];
                Pinecone::HTMLNodeList li_as = lpChunk->evinceHTMLElement()->getElementsByTagName( "a" );

                if( li_as.empty() || li_as.size() != 345 ) {
                    this->mpParentHeistum->trace( { "[Notice: Abnormal '<li>' Count]", i, li_as.size() } );
                }

                for ( int j = 0; j < li_as.size(); ++j ) {
                    Pinecone::HTMLNode* lpA = li_as[ j ];
                    Pinecone::HTMLElement* lpEA = lpA->evinceHTMLElement();

                    if( lpEA ) {
                        std::string szKey  = lpEA->innerText();
                        std::string szHref = lpEA->getAttribute( "href" );

                        jIndex.emplace_back( szHref );
                        ++nIndexCount;
                    }
                    else {
                        this->mpParentHeistum->error( { "[Compromised] At:", i, j } );
                    }

                }
            }

            this->mpParentHeistum->trace( { "[FileParsedSuccessful] At:", i, "FileSize:", szHtml.length() } );
        }

        std::string szIndexListPath = szIndexPath + "index_list.json5";
        std::ofstream oj( szIndexListPath, std::ios::binary );
        jIndex.serialize( oj );
        this->mpParentHeistum->trace( { "[IndexJsonifiedSuccessful] Path:", szIndexListPath, "IndexCount:", nIndexCount } );
    }

    void WikiReaver::embezzleFromRange() const {
        this->embezzle_wiktionary_from_range();
    }

    void WikiReaver::embezzle_wikipedia_solo_page( const std::string &szPageCache, Pinecone::Atom &joPage ) const {
        Pinecone::HTMLDocument document;
        Pinecone::HTMLParser   htmlParser ( szPageCache );
        htmlParser.parse( &document );

        std::string szWord;
        Pinecone::HTMLElement* lpWordE = document.querySelector( ".mw-page-title-main, .firstHeading.mw-first-heading, h1" );
        if( lpWordE ) {
            szWord = lpWordE->innerText();
        }
        else {
            this->parentHeistum()->error( { "[WikiReaver] Abnormal word title, miss in .mw-page-title-main, h1." } );
            return;
        }
    }

    void WikiReaver::embezzle_wikipedia_solo_page_to_sql( const std::string &szPageCache ) const {
        std::string szThisSQL;
        szThisSQL.reserve( PINE_MB(2) );

        Pinecone::Atom joPage;
        this->embezzle_wikipedia_solo_page( szPageCache, joPage );
    }



    void WikiReaver::embezzle_wiktionary_from_range() const {
        std::string szSQLPath = this->parentHeistum()->queryConfig( "SQLPath" )->toString();
        this->mofSeg.open( szSQLPath + this->queryFragNamespace(this->mnLootFrom) + ".sql" , std::ios::binary );
        for ( size_t i = this->mnLootFrom; i < this->mnLootTo; ++i ) {
            try {
                this->embezzle_wiktionary_solo_page_by_id( i );
                this->thisom[ "id" ] = (reg_t) i;
                //echo i, endp;
                this->mpParentHeistum->noticeLootDone( this, i, false );
            }
            catch ( Pinecone::IllegalStateException& e ) {
                this->mpParentHeistum->error( { i, e.getMessage() } );
            }
        }
    }

    void WikiReaver::embezzle_wiktionary_solo_page( const std::string& szPageCache, Pinecone::Atom& joPage ) const {
        Pinecone::HTMLDocument document;
        Pinecone::HTMLParser   htmlParser ( szPageCache );
        htmlParser.parse( &document );
        Pinecone::Atom& joTagProperties = *(Pinecone::Atom*)this->parentHeistum()->queryConfig( "WordTypeProperties" );

        typedef std::function<bool ( Pinecone::HTMLElement* ) > fnElmQualified_t;

        auto fnIsBasicEscapeEle = []( Pinecone::HTMLElement* lpE ) {
            return
            lpE->elementType() == Pinecone::HTMLElement::ElementType::T_ELE_STYLE ||
            lpE->elementType() == Pinecone::HTMLElement::ElementType::T_ELE_SCRIPT;
        };

        std::function<bool ( Pinecone::HTMLElement*, Pinecone::Atom&, const fnElmQualified_t& )> fnEvalCommonOUL =
                [&]( Pinecone::HTMLElement* lpE, Pinecone::Atom& jo, const fnElmQualified_t& fnElmQualified ) -> bool {
            if( lpE->elementType() == Pinecone::HTMLElement::ElementType::T_ELE_OL || lpE->elementType() == Pinecone::HTMLElement::ElementType::T_ELE_UL ){
                Pinecone::HTMLNodeList nodeList = lpE->getElementsByTagName( "li" );
                if( !nodeList.empty() ){
                    for ( int i = 0; i < nodeList.size(); ++i ) {
                        Pinecone::HTMLElement* lpLi = nodeList[ i ]->evinceHTMLElement();
                        if( fnElmQualified( lpLi ) ) {
                            jo.affirmDictron()->affirmListron()->emplace_back( StringHelper::trimFront( NonaHelper::innerTextNoScriptStyle( lpLi ) ) );
                        }
                    }
                }
                return true;
            }
            else if( lpE->elementType() == Pinecone::HTMLElement::ElementType::T_ELE_DIV ){
                Pinecone::DOMTreeSelector treeSelector( lpE, "ol,ul" );
                Pinecone::HTMLElement* lpL = treeSelector.querySelector();
                if( lpL ) {
                    return fnEvalCommonOUL( lpL, jo, fnElmQualified );
                }
            }
            return false;
        };

        auto fnIsTitle = [] ( std::string szNodeName ) {
            return szNodeName.front() == 'h' && szNodeName.size() == 2;
        };

        auto fnReadTitle = [&]( Pinecone::HTMLElement* lpE ) ->std::string{
            Pinecone::HTMLNodeList spans = lpE->getElementsByClassName( "mw-headline" );
            if( !spans.empty() ) {
                Pinecone::HTMLElement* lpSpan = spans[0]->evinceHTMLElement();
                return StringHelper::trimFront( lpSpan->innerText() );
            }
            return "";
        };

        auto fnEvalWordTagProperties = [&]( Pinecone::HTMLElement* lpE, Pinecone::Atom& joProperties ) {
            std::string szSecTitle = fnReadTitle( lpE );
            if( joTagProperties.hasOwnProperty( szSecTitle ) ) {
                Pinecone::Atom& jPro = joTagProperties[ szSecTitle ];
                Pinecone::Listron& listron = *joProperties.affirmDictron()->affirmListron();
                if( std::find( listron.begin(), listron.end(), jPro ) == listron.end() ){
                    listron.emplace_back( jPro );
                }
            }
        };

        auto fnEvalBasicNode = [&] ( Pinecone::HTMLElement*& lpE, const std::string& szTitle, Pinecone::Atom& jo ) {
            Pinecone::Atom& joList = jo[ "$List" ];
            if( !jo.hasOwnProperty( "$SimpleList" ) ){
                jo[ "$SimpleList" ] = true;
            }

            bool isOUL = fnEvalCommonOUL( lpE, joList, []( Pinecone::HTMLElement* lpLi ){
                return true;
            } );
            if( !isOUL ) {
                jo[ "$SimpleList" ] = false;
                Pinecone::Atom joHTML;
                joHTML[ "Type" ] = "HTML";
                joHTML[ "Html" ] = lpE->innerHTML();
                joList.affirmDictron()->affirmListron()->emplace_back( std::move( joHTML ) );
            }
        };

        auto fnEvalPronunciationNode = [&] ( Pinecone::HTMLElement*& lpE, const std::string& szTitle, Pinecone::Atom& jo ) {
            Pinecone::Atom& joList = jo[ "$List" ];
            bool isOUL = fnEvalCommonOUL( lpE, joList, []( Pinecone::HTMLElement* lpLi ){
                std::string szText = Pinecone::StdString::toLowerCase( StringHelper::trimFront( NonaHelper::innerTextNoScriptStyle(lpLi) ) );
                return szText.find( "audio" ) == std::string::npos;
            } );
            if( !isOUL ) {
                Pinecone::Atom joHTML;
                joHTML[ "Type" ] = "HTML";
                joHTML[ "Html" ] = lpE->innerHTML();
                joList.affirmDictron()->affirmListron()->emplace_back( std::move( joHTML ) );
            }
        };

        std::function<Pinecone::HTMLElement* ( Pinecone::HTMLElement*&, Pinecone::Atom& )> fnEvalSubSection =
                std::function<Pinecone::HTMLElement* ( Pinecone::HTMLElement*&, Pinecone::Atom& )>(
        [&] ( Pinecone::HTMLElement*& lpE, Pinecone::Atom& jo ) {
            std::string szNodeName = Pinecone::StdString::toLowerCase( lpE->nodeName() );
            reg_t nHeadLevel = _PINE_ATOLL( szNodeName.c_str() + 1 );
            std::string szSecTitle = fnReadTitle( lpE );
            Pinecone::Atom& thisSection = jo[ szSecTitle ];
            thisSection[ "$Title" ] = szSecTitle;
            std::string szSecHTML;
            szSecHTML.reserve( PINE_KB(128) );

            reg_t nChildren = 0;
            while ( true ) {
                lpE = lpE->nextElementSibling();
                if( !lpE ) {
                    break;
                }
                if( fnIsBasicEscapeEle( lpE ) ) {
                    continue;
                }
                szSecHTML.append( lpE->innerHTML() );

                std::string szThisNodeName = Pinecone::StdString::toLowerCase( lpE->nodeName() );
                reg_t nThisHeadLevel = _PINE_ATOLL( szThisNodeName.c_str() + 1 );
                if( fnIsTitle( szThisNodeName ) ) {
                    ++nChildren;
                    fnEvalWordTagProperties ( lpE, joPage[ "TagProperties" ] );

                    if( nThisHeadLevel > nHeadLevel ) {
                        fnEvalSubSection( lpE, jo[ szSecTitle ] );
                        if( !lpE ) {
                            break;
                        }
                    }
                    else {
                        lpE = lpE->previousElementSibling();
                        break;
                    }
                }
                else {
                    if( Pinecone::StdString::startsWith( szSecTitle, "Pronunciation", 0 ) ){
                        fnEvalPronunciationNode ( lpE, szSecTitle, thisSection );
                    }
                    else if( Pinecone::StdString::startsWith( szSecTitle, "Etymology", 0 ) ){
                        Pinecone::Atom& joContent = thisSection[ "$Content" ];
                        joContent[ "Type" ] = "HTML";
                        joContent[ "Html" ].affirmString();
                        joContent[ "Html" ] += lpE->innerHTML();
                    }
                    else {
                        fnEvalBasicNode ( lpE, szSecTitle, thisSection );
                    }
                }
            }
            //thisSection[ "$RawHTML" ] = szSecHTML;
            thisSection[ "$ChildrenSize" ] = nChildren;
            //jo[ szSecTitle ];

            return lpE;
        } );

        auto fnEvalSeeAlsoRelatedWords = [&] ( Pinecone::Atom& jo ) {
            Pinecone::HTMLNodeList nodeList = document.querySelectorAll( ".disambig-see-also,.disambig-see-also-2 a" );
            if( !nodeList.empty() ){
                for ( int i = 0; i < nodeList.size(); ++i ) {
                    Pinecone::HTMLElement* lpE = nodeList[ i ]->evinceHTMLElement();
                    jo[ "SeeAlsoWords" ].affirmDictron()->push_back( StringHelper::trimFront( NonaHelper::innerTextNoScriptStyle(lpE) ) );
                }
            }
            else {
                jo[ "SeeAlsoWords" ].setNull();
            }
        };

        std::string szWord;
        Pinecone::HTMLElement* lpWordE = document.querySelector( ".mw-page-title-main, .firstHeading.mw-first-heading, h1" );
        if( lpWordE ) {
            szWord = lpWordE->innerText();
        }
        else {
            this->parentHeistum()->error( { "[WikiReaver] Abnormal word title, miss in .mw-page-title-main, h1." } );
            return;
        }

        joPage[ "Word" ] = szWord;
        Pinecone::HTMLElement* lpContent = document.querySelector( ".mw-parser-output" );
        if( lpContent ) {
            Pinecone::HTMLElement* lpE = lpContent->firstElementChild();
            bool bSecFound = false;
            Pinecone::Atom joSections;
            fnEvalSeeAlsoRelatedWords( joSections );

            while ( lpE ) {
//                Pinecone::HTMLElement::ElementType elementType = lpE->elementType();
//                if( elementType != Pinecone::HTMLElement::ElementType::T_ELE_H2 ) {
//                    if( fnIsTitle( lpE->nodeName() ) ) {
//                        std::string szTitle = fnReadTitle( lpE );
//                        if( !Pinecone::StdString::startsWith( szTitle, "Etymology", 0 ) ){
//                            this->thisom[ szTitle ];
//                        }
//                    }
//                }

                Pinecone::HTMLElement::ElementType elementType = lpE->elementType();
                if( elementType == Pinecone::HTMLElement::ElementType::T_ELE_H2 ) {
                    bSecFound = true;
                    lpE = fnEvalSubSection( lpE, joSections[ "Languages" ] );
                    if( !lpE ) {
                        break;
                    }
                }
                lpE = lpE->nextElementSibling();
            }

            if( !bSecFound ) {
                this->parentHeistum()->error( { "[WikiReaver] Abnormal section inlet in .mw-parser-output." } );
            }

            joPage[ "Sections" ] = std::move( joSections );
            //std::ofstream ofs ( "\\\\B-ServerKingpin\\ARBOmnium\\EnderChest\\Facility/a.json5", std::ios::binary );
            //joPage.serialize( ofs, 0, 4 );
            //echo joPage.toJSONString(4);
        }
        else {
            this->parentHeistum()->error( { "[WikiReaver] Empty .mw-parser-output." } );
        }
    }

    void WikiReaver::embezzle_wiktionary_solo_page_to_sql( const std::string &szPageCache ) const {
        std::string szThisSQL;
        szThisSQL.reserve( PINE_MB(2) );

        Pinecone::Atom joPage;
        this->embezzle_wiktionary_solo_page( szPageCache, joPage );

        Pinecone::Atom& joTagProperties = *(Pinecone::Atom*)this->parentHeistum()->queryConfig( "WordTypeProperties" );
        std::string szToken       = joPage[ "Word" ].toString();
        std::string szSToken      = Pinecone::SQLString::escape( szToken );
        std::string szSLongToken  ;
        if( szSToken.length() > 250 ) {
            szSLongToken = std::move( szSToken );
            szSToken     = szSLongToken.substr( 0, 250 );
        }
        std::string szCurLang ;

        std::map<std::string, Pinecone::Atom > joTokens;
        joTokens[ "token"            ]   = szSToken;
        joTokens[ "long_token"       ]   = szSLongToken;
        joTokens[ "prime_refer_href" ]   = "";
        std::string szPrimeHref = Pinecone::SQLString::escape( "https://en.wiktionary.org/wiki/" + Pinecone::HTTPURLParser::urlEncode( szToken ) );
        if( szPrimeHref.length() < 330 ) {
            joTokens[ "prime_refer_href" ]  = std::move( szPrimeHref );
        }
        szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_tokens", joTokens ) ) += ";\n";

        auto fnSQLifyKVArrayLike = [&]( Pinecone::Atom& json, const char* lpszKey, const char* lpszTab ){
            Pinecone::Listron& ja = *json.affirmDictron()->affirmListron();
            for ( auto& row : ja ) {
                std::map<std::string, Pinecone::Atom > jo;
                jo[ "token"    ]   = szSToken;
                jo[ lpszKey    ]   = Pinecone::SQLString::escape( row.toString() );
                szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( lpszTab, jo ) ) += ";\n";
            }
        };
        fnSQLifyKVArrayLike( joPage[ "TagProperties" ], "property", "spe_dict_tag_prop" );
        fnSQLifyKVArrayLike( joPage[ "Sections" ][ "SeeAlsoWords" ], "relate_token", "spe_dict_relate_tokens" );

        auto fnSQLifyPronunciation = [&]( Pinecone::Maptron::T_KeyValue& kv ){
            Pinecone::Listron& ja = *kv.Value[ "$List" ].affirmDictron()->affirmListron();
            for ( auto& row : ja ) {
                std::map<std::string, Pinecone::Atom > joSQL;
                joSQL[ "token"            ] = szSToken;
                joSQL[ "language"         ] = szCurLang;
                joSQL[ "ipa_phonetic"     ] = "";
                joSQL[ "other_phonetic"   ] = "";

                if( row.isDictron() ) {
                    std::string szHTML = row[ "Html" ].toString();
                    joSQL[ "other_phonetic" ] = Pinecone::SQLString::escape( szHTML );
                }
                else if( row.isString() ) {
                    std::string szPron = row.toString();
                    if( szPron.length() > 330 ) {
                        joSQL[ "other_phonetic" ] = Pinecone::SQLString::escape( szPron );
                    }
                    else {
                        if( Pinecone::StdString::startsWith( szPron, "IPA ( key):", 0 ) ) {
                            szPron = Pinecone::StringStereotype::tokenCursorSearch( szPron.c_str(), "IPA ( key):", false );
                        }
                        else if( Pinecone::StdString::startsWith( szPron, "IPA (key):", 0 ) ) {
                            szPron = Pinecone::StringStereotype::tokenCursorSearch( szPron.c_str(), "IPA (key):", false );
                        }

                        szPron = StringHelper::trimFront( szPron );
                        szPron = StringHelper::utf8MB3Normalize( szPron.c_str(), szPron.length() );
                        joSQL[ "ipa_phonetic"     ] = Pinecone::SQLString::escape( szPron );
                    }
                }

                szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_phonetic", joSQL ) ) += ";\n";
            }
        };

        auto fnSQLifyDefinitions = [&]( Pinecone::Maptron::T_KeyValue& kv ){
            std::string szStandTag = joTagProperties[ kv.Key ].toString();
            Pinecone::Maptron& jo = *kv.Value.affirmDictron()->affirmMaptron();
            for( auto& pair : jo ) {
                if( pair.Key == "$List" ) {
                    Pinecone::Listron& ja = *pair.Value.affirmDictron()->affirmListron();
                    for ( auto& row : ja ) {
                        std::map<std::string, Pinecone::Atom > joSQL;
                        joSQL[ "token"            ] = szSToken;
                        joSQL[ "property"         ] = szStandTag;
                        joSQL[ "language"         ] = szCurLang;
                        joSQL[ "w_definition"     ] = "";

                        if( row.isDictron() ) {
                            std::string szHTML = row[ "Html" ].toString();
                            joSQL[ "w_definition" ] = Pinecone::SQLString::escape( szHTML );
                        }
                        else if( row.isString() ) {
                            joSQL[ "w_definition" ] = Pinecone::SQLString::escape( row.toString() );
                        }

                        szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_en_definitions", joSQL ) ) += ";\n";
                    }
                }
                else {
                    if( !pair.Value.isDictron() ){
                        continue;
                    }
                    Pinecone::Listron& ja = *pair.Value[ "$List" ].affirmDictron()->affirmListron();
                    for ( auto& row : ja ) {
                        std::map<std::string, Pinecone::Atom > joSQL;
                        joSQL[ "token"            ] = szSToken;
                        joSQL[ "know_key"         ] = pair.Key;
                        joSQL[ "property"         ] = szStandTag;
                        joSQL[ "language"         ] = szCurLang;
                        joSQL[ "terms"            ] = "";
                        joSQL[ "w_definition"     ] = "";

                        if( row.isDictron() ) {
                            std::string szHTML = row[ "Html" ].toString();
                            joSQL[ "w_definition" ] = Pinecone::SQLString::escape( szHTML );
                        }
                        else if( row.isString() ) {
                            std::string szTerm = Pinecone::SQLString::escape( row.toString() );
                            if( szTerm.length() > 250 ) {
                                joSQL[ "w_definition" ] = szTerm;
                            }
                            else {
                                joSQL[ "terms"        ] = szTerm;
                            }
                        }

                        szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_define_extend_knowledge", joSQL ) ) += ";\n";
                    }
                }
            }


        };

        auto fnSQLifyLangSection = [&]( Pinecone::Atom& seg ){
            Pinecone::Maptron& jo = *seg.affirmDictron()->affirmMaptron();
            for ( auto& kv : jo ) {
                if( Pinecone::StdString::startsWith( kv.Key, "Pronunciation", 0 ) ) {
                    fnSQLifyPronunciation( kv );
                }
                else if( joTagProperties.hasOwnProperty( kv.Key ) ) {
                    fnSQLifyDefinitions( kv );
                }
                else if( kv.Key.front() != '$' ){
                    bool bIsSophisticatedKnow = true;
                    if( kv.Value[ "$ChildrenSize" ].getRawInt64Zone() == 1 ) {
                        if( kv.Value[ "$SimpleList" ].getRawInt64Zone() ) {
                            Pinecone::Atom& jaList = kv.Value[ "$List" ];
                            std::string szLocalSQL;
                            bIsSophisticatedKnow = false;
                            for ( int i = 0; i < jaList.size(); ++i ) {
                                std::string szItem = jaList[ i ].toString();
                                if( szItem.length() > 250 ) {
                                    bIsSophisticatedKnow = true;
                                    break;
                                }
                                std::map<std::string, Pinecone::Atom > joSQL;
                                joSQL[ "token"            ] = szSToken;
                                joSQL[ "term_key"         ] = kv.Key;
                                joSQL[ "language"         ] = szCurLang;
                                joSQL[ "relate_terms"     ] = Pinecone::SQLString::escape( szItem );
                                szLocalSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_relate_terms", joSQL ) ) += ";\n";
                            }
                            szThisSQL.append( szLocalSQL );
                        }
                    }

                    if( bIsSophisticatedKnow ) {
                        std::map<std::string, Pinecone::Atom > joSQL;
                        joSQL[ "token"            ] = szSToken;
                        joSQL[ "know_key"         ] = kv.Key;
                        joSQL[ "language"         ] = szCurLang;
                        joSQL[ "w_definition"     ] = Pinecone::SQLString::escape( kv.Value.toJSONString() );
                        szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_token_extend_knowledge", joSQL ) ) += ";\n";
                    }
                }
            }
        };

        Pinecone::Atom& jLanguages = joPage[ "Sections" ][ "Languages" ];
        Pinecone::Maptron& joLanguages = *jLanguages.affirmDictron()->affirmMaptron();
        for ( auto& kv : joLanguages ) {
            szCurLang = Pinecone::SQLString::escape( kv.Key );
            std::map<std::string, Pinecone::Atom > joLang;
            joLang[ "token"    ]   = szSToken;
            joLang[ "language" ]   = szCurLang;
            szThisSQL.append( Pinecone::MappedSQLSplicer::spliceInsertSQL ( "spe_dict_language", joLang ) ) += ";\n";

            fnSQLifyLangSection( kv.Value );
        }

        this->mofSeg << szThisSQL;
//        std::ofstream ofs ( "\\\\B-ServerKingpin\\ARBOmnium\\EnderChest\\Facility/a.sql", std::ios::binary );
//        ofs << szThisSQL;
//        echo Pinecone::StringCoding::UTF8ToGBK( szThisSQL );
    }

    void WikiReaver::embezzle_wiktionary_solo_page_by_id( size_t id ) const {
        std::string szStorageDir  = this->querySpoilStorageDir( id );
        std::string szStoragePath = this->querySpoilStoragePath( id );
        std::ifstream ifs ( szStoragePath, std::ios::binary );
        if( ifs.is_open() ) {
            std::string szCache = Pinecone::FileUtils::readAll( ifs );
            this->embezzle_wiktionary_solo_page_to_sql( szCache );
        }
        else {
            this->parentHeistum()->error( { "[WikiReaver] Unable to open spoil file." } );
        }
    }

    void WikiReaver::lootById( size_t id ) const {
        this->JsonIndexReaver::lootById( id );
//        std::string szStorageDir  = this->querySpoilStorageDir( id );
//        std::string szStoragePath = this->querySpoilStoragePath( id );
//
//        if( !PathIsDirectoryA( szStorageDir.c_str() ) ){
//            CreateDirectoryA( szStorageDir.c_str(), nullptr );
//        }
//
//        if( id >= 3500000 ) {
//            exit(0);
//        }
//
//        std::string szLegacyPath = this->mszSpoilPath + "Wikti_pages_250W_350W/page_" + Pinecone::StdString::toString( id ) + ".html";
//        MoveFileA( szLegacyPath.c_str(), szStoragePath.c_str() );
    }

    void WikiReaver::toEmbezzle() const {
        //( (WikiReaver*)this )->mnLootFrom = 0;
        //( (WikiReaver*)this )->mnLootTo = 100000;
        this->embezzle_wiktionary_from_range();


//        std::string& szSpoilPath   = this->mpParentHeistum->mszSpoilPath;
//        std::string  szFN = "\\\\B-ServerKingpin/ARBOmnium/EnderChest/Facility/page_473688.html";
//        std::ifstream ifs ( szFN, std::ios::binary );
//        std::string szHtml = Pinecone::FileUtils::readAll( ifs );
//        this->embezzle_wikipedia_solo_page_to_sql( szHtml );

    }

}