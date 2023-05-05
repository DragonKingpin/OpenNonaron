#ifndef NONARON_BROWSERSIM_H
#define NONARON_BROWSERSIM_H
#include <cstddef>
#include <string>
#include <functional>

namespace Pinecone {
    class Atom;
}

namespace Sauron {
    class System;

    _PINE_INTERFACE HTTPReaver;

    _PINE_ABSTRACT_CLASS BrowserSim {
    public:
        typedef       Pinecone::Atom        PineAtom  ;
        typedef       std::function<int ( char*, size_t )> fnAfterBufferReceived_t;

    public:
        virtual ~BrowserSim() = default;

    public:
        virtual void getAccept( const std::string &szHref, fnAfterBufferReceived_t&& fnAfterBufferReceived ) const = 0;

        virtual void getAccept( const std::string &szHref, std::ostream& ofs ) const = 0;

        virtual std::string getHTTPFile( const std::string &szHref ) const ;

    };

    class InnerBrowserSim : _PINE_EXTENDS public BrowserSim {
    private:
        PineAtom*     mpProtoConfig         = nullptr ;
        PineAtom*     mpHeadConfigGroup     = nullptr ;
        bool          mbGroupConfusion      = true    ;
        bool          mbEnableRandomDelay   = false   ;
        int32_t       mnRandomDelayMin      = 1000    ;
        int32_t       mnRandomDelayMax      = 2500    ;

    public:
        explicit InnerBrowserSim ( System* lpSys );

        explicit InnerBrowserSim ( HTTPReaver* lpReaver );

        InnerBrowserSim* apply( PineAtom* lpConf );

        ~InnerBrowserSim() _PINE_OVERRIDE = default;

    protected:
        virtual void beforeNextHttpQuery() const ;

        virtual void afterLastHttpQuery() const ;

    public:
        void getAccept( const std::string &szHref, fnAfterBufferReceived_t&& fnAfterBufferReceived ) const _PINE_OVERRIDE;

        void getAccept( const std::string &szHref, std::ostream& ofs ) const _PINE_OVERRIDE;

        std::string getHTTPFile( const std::string &szHref ) const _PINE_OVERRIDE;


    };
}

#endif //NONARON_BROWSERSIM_H
