#ifndef MTXScreen_h
#define MTXScreen_h


// There is nothing beyond the common implementation, so this class
// is effectively empty
class Screen : public BaseScreen {
public:
  virtual void display(bool force = false) = 0;
  virtual void processPeriodicActivity() = 0;

protected:

private:

};

#endif // MTXScreen_h
