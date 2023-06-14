#ifndef __LOCK_HELPER_H__
#define __LOCK_HELPER_H__

namespace lock_helper
{

/**
* Generic pure lock key class
*
*/
class CLockKey
{
public:
    /** Lock key */
    virtual void lock() {};
    /** Unlock key */
    virtual void unlock() {};
    /** Destructor */
    virtual ~CLockKey() {};
};

/**
* Automatic locker class
* 
* Use lock()/unlock() to lock/unlock the key
*
*/
class CAutoLock
{
public:
    /**
    * CLock constructor
    * Key is locked 
    * 
    * \param[in]   p_Key   Pointer to the key
    *
    */
    CAutoLock(CLockKey* key, bool b_auto = true)
    {
        m_p_Key = key;
        m_b_auto = b_auto;
        if (m_b_auto)
            m_p_Key->lock();
    }

    /**
    * CLock constructor
    * Key is locked 
    * 
    * \param[in]   p_Key   Pointer to the key
    *
    */
    CAutoLock(CLockKey& key, bool b_auto = true)
    {
        m_p_Key = &key;
        m_b_auto = b_auto;
        if (m_b_auto)
            m_p_Key->lock();
    }
    
    /**
     * Destructor (unlocks the key)
     *
     */
    virtual ~CAutoLock()
    {
        if (!m_b_auto)
            return;
        if (m_p_Key)
            m_p_Key->unlock();
    }

private:
    /** Pointer to the key */
    CLockKey* m_p_Key;
    bool m_b_auto;
};

/**
* Boolean key class
* 
*/
class CLockKey_bool : public CLockKey
{
public:
    /**
     * Contructor
     *
     */
    CLockKey_bool()
    {
        m_key = false;
    }

    /**
     * Wait until key is locked and then (re)locks the key
     *
     */
    virtual void lock()
    {
        
        while(m_key);
        m_key = true;
    }
    
    /**
     * Unlock the key
     *
     */
    virtual void unlock()
    {
        m_key = false;
    }

private:
    /** The key */
    bool m_key;
};

#ifdef _WIN32

/**
* Critical Section key class (Windows platforms)
* 
*/
class CLockKey_CS : public CLockKey
{
public:
    /**
     * Constructor
     *
     */
    CLockKey_CS()
    {
        ::InitializeCriticalSection(&m_sec);
    }

    /**          
     * Destructor
     *  
     */          
    virtual ~CLockKey_CS() 
    {
        ::DeleteCriticalSection(&m_sec);
    }

    /**
     * Lock key
     *
     */
    virtual void lock()
    {
        ::EnterCriticalSection(&m_sec);        
    }

    /**
     * Unlock key
     *
     */
    virtual void unlock()
    {
        ::LeaveCriticalSection(&m_sec);
    }

private:
   /** The key (critical section handler) */
   CRITICAL_SECTION m_sec;
};

#else

#include <pthread.h>
#include <semaphore.h>

/**
* Thread lock key class with mutex
* 
*/
class CLockKey_Thread : public CLockKey
{
public:
    /**
     * Constructor (initializes internal mutex)
     *
     */
    CLockKey_Thread()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    /** 
     * Destructor (destroys internal mutex)
     *  
     */              
    virtual ~CLockKey_Thread()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    
    /**
     * Lock key
     *
     */
    virtual void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    
    /**
     * Unlock key
     *
     */
    virtual void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    /** The key */
    pthread_mutex_t m_mutex;
};

/**
* Thread lock key class with mutex and conditional variable
* 
*/
class CLockKey_Thread_Cond : public CLockKey
{
public:
    /**
     * Constructor (initializes internal mutex and condition)
     *
     */
    CLockKey_Thread_Cond()
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
        m_b_lock = false;
    }

    /** 
     * Destructor (destroys internal mutex and condition)
     *  
     */
    virtual ~CLockKey_Thread_Cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    /**
     * Wait on internal condition variable
     *
     */
    virtual void lock()
    {
        pthread_mutex_lock(&m_mutex);
        m_b_lock = true;
        pthread_cond_wait(&m_cond, &m_mutex);
        pthread_cond_init(&m_cond, NULL);
        m_b_lock = false;
        pthread_mutex_unlock(&m_mutex);
    }
    
    /**
     * Signal thread waiting on internal conditions variable
     *
     */
    virtual void unlock()
    {
        pthread_mutex_lock(&m_mutex);
        if (m_b_lock)
            pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
    
private:
    /** The keys */
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
    bool            m_b_lock;
};

/**
* Lock key class with semaphore
* 
*/
class CLockKey_Semaphore : public CLockKey
{
public:
    /**
     * Constructor (initializes internal semaphore variable) 
     *
     */
    CLockKey_Semaphore()
    {
        sem_init(&m_sem, 0, 1);
    }

    /**
     * Destructor (destroys internal semaphore variable)
     *
     */
    virtual ~CLockKey_Semaphore()
    {
         sem_destroy(&m_sem);
    }

    /**
     * Increment semaphore value
     *
     */
    virtual void lock()
    {
        sem_wait(&m_sem);
    }
    
    /**
     * Acquire semaphore
     *
     */
    virtual void wait()
    {
        sem_trywait(&m_sem);
    }

    /** 
     * Decrement semaphore value
     *   
     */ 
    virtual void unlock()
    {
        sem_post(&m_sem);
    }
    
private:
    /** The key */
    sem_t  m_sem; 
};

#endif

}

#endif /*__LOCK_HELPER_H__*/
