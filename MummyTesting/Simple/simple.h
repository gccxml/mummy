// comment
class simple
{
public:
  simple();
  virtual ~simple();

  void MethodToWrap();

  int m_PublicData;

protected:
  void MethodNotToWrap();
  int m_ProtectedData;

private:
  int m_PrivateData;
};
